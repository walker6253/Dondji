/*
 * Dondji Firmware — MDC1200 RX application layer
 *
 * [软接收 2026-10-08] 移植自 V9 固件(两机互收 MDC ID 实机验证 2026-10-08):
 *   硬件 FSK RX 全退场(匹配字/侧车/重臂/8相位重对齐全删) — BK4819 保持
 *   普通 FM RX, 不配置 FSK RX 块。
 *   信号链: PA4 模拟音频(AC耦合, MCU DAC 无缓冲输出钳位中点2048)
 *   → ADC 通道4 @14.4kHz(TIM3 TRGO 外部触发, EOC 中断逐样本)
 *   → 软件解调前端(带通1469.7Hz + 24样本窗滑动相关器 + DPLL)
 *   → 空中原始bit流(不做NRZI) → 64字节位环(ISR→主循环)
 *   → 128字节捕获缓冲(入栈即XOR 0xFF转解码域) → losehu 滑窗解码链(逐bit滑窗搜40bit
 *   同步模式, 32/40容错, 正/反极性双分支) → ID 弹窗显示。
 *
 *   [采样率 14400Hz 2026-10-09] 14400=1200×12=1800×8, 两音调均整周期采样,
 *   余弦表单周期无截断(cm1200[12]/cs1800[8]); 窗24@14400=1.667ms(与原
 *   窗16@9600等长), 第一零点=14400/24=600Hz=频差→1200/1800数学正交串扰=0。
 *   BP系数按Fs=14400 RBJ重算; 每bit 12样本, PLL_STEP=5461(61ppm欠量由DPLL
 *   闭环吸收); 相关器O(1)增量更新, ISR计算量与窗长无关。
 *
 *   窗长正交论证: 8样本窗@9600主瓣宽1200Hz, 1800距1200仅600Hz仍在主瓣内
 *   →串扰64%, 叠加FM去加重幅度差→bit1判决余量仅~4%必败; 等窗时长1.667ms
 *   方案(16@9600 / 24@14400)第一零点=600Hz=频差→串扰=0, 幅度差彻底免疫。
 *   音调-bit映射: 1200Hz=空中bit 1 (即使映射颠倒,
 *   差分编码对全局取反免疫, 第二遍翻转首bit解码覆盖残余奇偶态)。
 *
 * 门控: 静噪开(绿灯亮)时采样解码, 静噪关时停止+收尾解码; TX/频谱/CW/
 *   Yan ID RX 期间停采样; FM 收音机模式不采样。自收抑制: 时间窗 +
 *   ID 指纹双条件 — 只丢 ID==自身的 TX 尾残留回声; 窗内他人真实帧放行,
 *   窗外自 ID 回声(中继延迟 echo)照常显示。
 * 电池采样门控(BOARD_ADC_GetBatteryInfo): MDC 软接收采样期间跳过 —
 *   ADC 在外部触发模式下 SW 触发的电池采样会死等 EOS。
 * TX 侧(BK4819_PlayMDC1200)保持 losehu 标准硬件 FSK 形态, 不变。
 * 参考固件: uv-k1-k5v3-firmware-custom (F4HWN fork, 同硬件,
 *   PA4 音频通路+前端结构已实机验证)。
 *
 * [尺寸优化] (FLASH 贴 118K 上限):
 *   - 相关器 4 MAC 走 cv[4] 系数栈表+循环, 表相位由环槽 r 条件减法派生
 *     (14400 下单周期表 cm1200[12]/cs1800[8] 合计20B, 比双16表省16B);
 *   - PPRE 分支删除: 全工程从不写 RCC_CFGR_PPRE(复位值0), 定时器时钟
 *     恒等于 SystemCoreClock, 分支为运行时死代码;
 *   - 采样停止改为调 BOARD_ADC_Init() 整态恢复(含校准, µs级), 替代
 *     逐寄存器保存/恢复 — BOARD_ADC_Init 幂等, 结果与恢复等价;
 *   - PA4/DAC1/TIM3 空闲态恢复硬编码(全预设 ENABLE_VOICE=false, 固件
 *     无他人使用 PA4/DAC1/TIM3; 键盘列在 PB3-6 不在 PA4);
 *   - 捕获缓冲入栈时即 XOR 0xFF, 省去解码前整段拷贝;
 *   - ENABLE_VOICE=true 时软接收整体禁用(PA4 被 VOICE 占用, 二者互斥)。
 */
#include "app/mdc1200.h"
#include "app/mdc1200_app.h"
#include "app/mdc_addrbook.h"
#include "app/yan_id_rf.h"
#include "board.h"
#include "functions.h"
#include "misc.h"
#include "settings.h"
#ifdef ENABLE_FMRADIO
    #include "app/fm.h"
#endif
#include "py32f0xx.h"
#include <string.h>

uint16_t gMdcId_RX;
uint8_t  gMdcId_RX_timeout;
char     gMdcCallsign[MDC_ADDRBOOK_NAME_LEN + 1];

/* ---- 解调常量 1200/1800 FFSK @Fs=14400Hz, 窗24 ---- */
/* 带通滤波器: 中心 sqrt(1200*1800)=1469.7Hz, Q=0.9, Q14定点
   (RBJ公式按Fs=14400重算) */
#define BP_B0       4087
#define BP_A1       (-19700)
#define BP_A2       8202
/* 相关器余弦表, 127幅度, 单周期(整周期采样无截断), 两表连续共用基址:
   [0..11] 1200Hz: 14400/1200=12样本/周期
   [12..19] 1800Hz: 14400/1800=8样本/周期
   正弦=余弦表相位-90°: 1200 -3样本(+9 mod12), 1800 -2样本(+6 mod8) */
static const int8_t mdc_c[20] = {
    127, 110, 64, 0, -64, -110, -127, -110, -64, 0, 64, 110,
    127, 90, 0, -90, -127, -90, 0, 90,
};
#define PLL_STEP    5461    /* 65536/bit ÷ 12样本/bit = 5461.33; 取整每bit欠
                               4/65536=61ppm, 由DPLL跳变修正闭环吸收 */
#define PLL_CTR     45056    /* 跳变锚点(bit内69%位置=8.32/12样本, 与原
                               5.5/8同位; 窗1.667ms群延迟补偿, bit率1200未变) */
#define PLL_SHIFT   2        /* 每次跳变修正1/4误差 */
#define AGC_ATT     5        /* 峰值跟踪器: 攻击1/32 */
#define AGC_DEC     10       /* 衰减1/1024每样本 */
#define PK_MIN      16

#define CAP_SIZE    128u     /* 捕获缓冲(字节) */
#define CAP_MIN     19u      /* 解码最少需求: FF×3+同步5+码字14(XOR后) */

/* ---- 解调前端状态 (仅ISR写) ---- */
typedef struct {
    int32_t  dc;                    /* DC跟踪器, Q4 */
    int32_t  x1, x2, y1, y2;       /* 带通状态 */
    int32_t  sum[4];               /* 滑动相关器和: [0][1]=1200Hz I/Q, [2][3]=1800Hz I/Q */
    int32_t  pm, ps;               /* 各音调幅度峰值(AGC) */
    uint32_t r;                    /* 环槽(24样本窗)/表相位 */
    int16_t  ring[24][4];          /* 24样本窗的乘积(=1.667ms: 1200两周期/
                                      1800三周期, 数学正交串扰=0) */
    int32_t  dprev, phase;         /* 判决值, DPLL相位(65536/bit) */
    uint8_t  acc, bits;            /* 字节累加器, bit计数 */
} mdc_dem_t;

static mdc_dem_t       s_dem;
static volatile bool   s_sampling;
static volatile uint8_t s_ring_head;
static volatile uint8_t s_ring_tail;
static uint8_t          s_ring[64];        /* ISR→主循环字节环(512bit≈427ms) */
static uint8_t         s_cap[CAP_SIZE];    /* 捕获缓冲(解码域: 入栈时已XOR 0xFF) */
static uint8_t         s_cap_len;

static bool     s_ignore_next_self_rx;
static uint8_t  s_ignore_self_ticks;    /* 500ms单位 */

/* 解码结果 (losehu解码链输出) */
static uint16_t s_decoded_id;

bool MDC1200_AppRxEnabled(void)
{
#ifdef ENABLE_VOICE
    return false;    /* PA4/DAC1 被 VOICE 占用, 软接收互斥不可用 */
#else
    return gEeprom.mdc_id_rx;
#endif
}

/* |(i, q)| ≈ max + 3/8·min (aprsrx同款) */
static int32_t mdc_mag(int32_t i, int32_t q)
{
    if (i < 0) i = -i;
    if (q < 0) q = -q;
    if (i < q) { const int32_t t = i; i = q; q = t; }
    return i + (q >> 2) + (q >> 3);
}

/* 单音调幅度+峰值跟踪器(AGC) (aprsrx同款) */
static int32_t mdc_tone(int32_t i, int32_t q, int32_t *pk)
{
    const int32_t mt = mdc_mag(i, q);
    int32_t p = *pk;
    p += mt > p ? (mt - p) >> AGC_ATT : -(p >> AGC_DEC);
    if (p < PK_MIN) p = PK_MIN;
    *pk = p;
    return mt;
}

/* ---- ADC采样中断: 14.4kHz TIM3 TRGO触发, 逐样本跑解调前端 ---- */
void ADC_COMP_IRQHandler(void)
{
    const int32_t adc = (int32_t)(ADC1->DR & 0xFFFu);   /* 读DR即清EOC
                                                            (唯一使能的中断源) */

    mdc_dem_t *m = &s_dem;

    /* 前端: DC去除 → 带通双二阶(Q14) */
    m->dc += ((adc << 4) - m->dc) >> 6;
    const int32_t x = adc - (m->dc >> 4);
    const int32_t y = (BP_B0 * (x - m->x2) - BP_A1 * m->y1 - BP_A2 * m->y2) >> 14;
    m->x2 = m->x1; m->x1 = x; m->y2 = m->y1; m->y1 = y;

    /* 滑动相关器, 24样本窗口; 正弦=余弦表相位-90°。表相位由环槽r派生
       (条件减法/位与, 免M0+软除法): 1200相位=r mod 12(窗24含2周期);
       1800相位=r mod 8(窗24是8的整倍数, 可位与); 表内偏移12 */
    uint32_t p12 = m->r;
    if (p12 >= 12u) p12 -= 12u;
    uint32_t q12 = p12 + 9u;                 /* Q1200: -90°=-3样本 */
    if (q12 >= 12u) q12 -= 12u;
    const uint32_t i8 = m->r & 7u;           /* 1800相位(窗24是8的整倍数) */

    int16_t *o = m->ring[m->r];
    const uint8_t ix[4] = {
        (uint8_t)p12, (uint8_t)q12,
        (uint8_t)(12u + i8), (uint8_t)(12u + ((i8 + 6u) & 7u)),  /* 1800 I,Q(-2样本) */
    };
    for (uint32_t j = 0; j < 4u; j++) {
        const int32_t p = (y * (int32_t)mdc_c[ix[j]]) >> 8;
        m->sum[j] += p - o[j];
        o[j] = (int16_t)p;
    }
    m->r++;
    if (m->r >= 24u) m->r = 0;

    const int32_t mm = mdc_tone(m->sum[0], m->sum[1], &m->pm);   /* 1200Hz幅度 */
    const int32_t ms = mdc_tone(m->sum[2], m->sum[3], &m->ps);   /* 1800Hz幅度 */

    /* 判决: 1200Hz占优为正 → 空中bit 1 (1200Hz=1映射, 差分免疫) */
    const int32_t d = mm * m->ps - ms * m->pm;

    /* DPLL: phase wrap时输出bit, 跳变拉中点 */
    m->phase += PLL_STEP;
    if ((d > 0) != (m->dprev > 0))
        m->phase += (PLL_CTR - m->phase) >> PLL_SHIFT;
    m->dprev = d;
    if (m->phase >= 65536) {
        m->phase -= 65536;
        m->acc = (uint8_t)((m->acc << 1) | (d > 0 ? 1u : 0u));   /* MSB先到 */
        if (++m->bits >= 8u) {
            m->bits = 0;
            const uint8_t nxt = (uint8_t)((s_ring_head + 1u) & 63u);
            if (nxt != s_ring_tail) {          /* 满: ISR侧丢字节 */
                s_ring[s_ring_head] = m->acc;
                s_ring_head = nxt;
            }
        }
    }
}

/* ---- 启动软接收: PA4偏置+TIM3触发+ADC通道4 ---- */
static void mdc_sampling_start(void)
{
    if (s_sampling)
        return;
    memset(&s_dem, 0, sizeof(s_dem));
    s_ring_head = 0;
    s_ring_tail = 0;
    s_cap_len = 0;

    /* PA4偏置: AC耦合音频无直流参考, MCU DAC无缓冲输出钳位中点
       (aprsrx同款已验证序列: EN1+BOFF1+TEN1+TSEL=111软件触发)
       PA4/DAC1/TIM3 平时空闲(无VOICE), 无需保存现场 */
    GPIOA->MODER  = (GPIOA->MODER & ~(3u << 8)) | (3u << 8);   /* PA4模拟 */
    RCC->APBENR1 |= RCC_APBENR1_DAC1EN | RCC_APBENR1_TIM3EN;
    DAC1->CR      = (1u << 0) | (1u << 1) | (1u << 2) | (7u << 3);
    DAC1->DHR12R1 = 2048;
    DAC1->SWTRIGR = 1;

    /* TIM3: 14.4kHz update→TRGO (仅出触发信号, 不开中断)
       PPRE全工程恒为复位值0(定时器时钟=SystemCoreClock=HSI 8MHz), 无需分频修正;
       ARR=554→实测14414Hz(+0.10%), 偏差由DPLL闭环吸收。
       TIM3为全新使能(CR1/PSC/ARPE均复位态): ARR直写即生效, 无需EGR重装 */
    TIM3->CR2 = TIM_CR2_MMS_1;            /* MMS=010: update→TRGO */
    TIM3->ARR = (SystemCoreClock / 14400u) - 1u;

    /* ADC: 切通道4(PA4), TIM3 TRGO外部触发 (停止时BOARD_ADC_Init整态恢复)
       直写(非读改写): 采样期间 ADC 无人共用(电池采样被门控), 且 BOARD_ADC_Init
       恢复全态; SMPR3/SQR3 其余位清0=复位值语义, CR1 复位值0 仅加 EOCIE */
    ADC1->CR2   = 0;                    /* 改配置前禁用+清触发模式 */
    ADC1->SMPR3 = 5u << 12;             /* ch4: 41.5周期(ADC clk=2MHz→27µs
                                           < 采样间隔69.4µs@14.4k, 余量充足) */
    ADC1->SQR3  = 4u;                   /* rank1=通道4 (L=1由SQR1复位值) */
    ADC1->SR    = 0;
    ADC1->CR1   = ADC_CR1_EOCIE;
    ADC1->CR2   = ADC_CR2_EXTSEL_2      /* EXTSEL=100: TIM3 TRGO */
               | ADC_CR2_EXTTRIG
               | ADC_CR2_ADON;          /* 首次转换等首个TRGO, 无立即转换 */

    /* NVIC 直写(等价 CMSIS 内联, 省其 DSB/ISB 序列): 优先级字段在高2bit;
       使能后首个转换最早在 TIM3 启动后 1/14400s, 使能生效延迟无碍 */
    NVIC->IP[ADC_COMP_IRQn]   = 2u << 6;
    NVIC->ISER[0]             = 1u << ADC_COMP_IRQn;

    TIM3->CR1 = TIM_CR1_CEN;                   /* 启动采样 (CR1仅CEN) */
    s_sampling = true;
}

/* ---- 停止软接收: 外设回空闲态 (不动s_cap, 由调用方处理) ---- */
static void mdc_sampling_stop(void)
{
    if (!s_sampling)
        return;
    s_sampling = false;

    /* 无需先停TIM3->CR1: 下面ADC先关ADON并由BOARD_ADC_Init切回SW触发,
       TIM3残余TRGO已无作用; 末尾关TIM3时钟(RCC)即停定时器 */
    NVIC->ICER[0] = 1u << ADC_COMP_IRQn;   /* 直写ICER(等价NVIC_DisableIRQ, 省DSB/ISB;
                                              残留迟到IRQ良性: s_sampling已false, start重置环) */
    ADC1->CR1 &= ~ADC_CR1_EOCIE;
    ADC1->CR2 &= ~ADC_CR2_ADON;

    /* ADC整态恢复(SW触发/通道8/41.5周期, 含校准µs级): 幂等, 等价逐寄存器恢复 */
    BOARD_ADC_Init();

    /* PA4/DAC1/TIM3 回空闲态(平时无人使用, 无需保存恢复) */
    DAC1->CR = 0;
    RCC->APBENR1 &= ~(RCC_APBENR1_DAC1EN | RCC_APBENR1_TIM3EN);
    GPIOA->MODER &= ~(3u << 8);           /* PA4回输入 */
}

/* ---- 捕获缓冲整段解码 (解码器无状态; s_cap已是解码域=空中字节XOR 0xFF) ----
   双遍解码: 帧前差分奇偶未知 → 翻转首bit再试 (翻转首bit=反转整个差分流,
   试完还原)。第二遍必须保留: 1200/1800→bit1 映射颠倒时的兜底。 */
static bool mdc_soft_try_decode(void)
{
    static uint8_t op, arg;    /* 解码器出参, 呼号/ID显示不消费 */
    if (MDC1200_process_rx_data(s_cap, s_cap_len, &op, &arg, &s_decoded_id))
        return true;
    s_cap[0] ^= 0x80u;
    const bool ok = MDC1200_process_rx_data(s_cap, s_cap_len, &op, &arg, &s_decoded_id);
    s_cap[0] ^= 0x80u;
    return ok;
}

/* 帧接受: 解码+自收抑制+触发显示 */
static void mdc_try_accept_rx(void)
{
    if (s_cap_len < CAP_MIN)
        return;
    if (!mdc_soft_try_decode())
        return;                                /* 失败静默丢弃 */

    /* 自收抑制: 时间窗+ID指纹双条件 — 只丢ID==自身的TX尾残留回声;
       窗内他人真实帧放行, 窗外自ID回声(中继延迟echo)照常显示 */
    if (s_ignore_next_self_rx && s_decoded_id == gMDC1200_ID) {
        s_ignore_next_self_rx = false;
        s_ignore_self_ticks = 0;
        return;
    }

    gMdcId_RX         = s_decoded_id;
    gMdcId_RX_timeout = 12; /* 6 s @ 500 ms — 同 Yan ID 弹窗窗口 */
    gMdcCallsign[0]   = 0;
    MDC_AddrBookLookup(s_decoded_id, gMdcCallsign);
    gUpdateDisplay = true;
}

/* ---- 泄流ISR位环 → 捕获缓冲, 适时尝试解码 ---- */
static void mdc_drain_ring(void)
{
    while (s_ring_tail != s_ring_head) {
        const uint8_t b = s_ring[s_ring_tail];
        s_ring_tail = (uint8_t)((s_ring_tail + 1u) & 63u);
        if (s_cap_len < CAP_SIZE)
            s_cap[s_cap_len++] = (uint8_t)(b ^ 0xFFu);   /* 入栈即转解码域 */
    }
    if (s_cap_len >= CAP_SIZE) {
        /* 满: 整段解码后滑窗保留后64字节 (任何≤64字节帧不丢失) */
        mdc_try_accept_rx();
        memmove(s_cap, s_cap + 64, 64);
        s_cap_len = 64;
    } else if (s_cap_len >= CAP_MIN && (s_cap_len & 15u) == 0u) {
        /* 每16新字节尝试一次 (帧尾场景) */
        mdc_try_accept_rx();
    }
}

/* TX开始/独立模式入口清扫: 停止软接收采样 (FUNCTION_Transmit /
   频谱 / CW 入口调用, TX期间与冻结tick的模式里不收自己) */
void MDC1200_AppDisableRx(void)
{
    if (s_sampling)
        mdc_sampling_stop();
    s_cap_len = 0;
}

/* MDC TX 结束(bk4829.c BK4819_PlayMDC1200尾部调用):
   设自收抑制窗, 只忽略自己的下一次接收(TX尾段残留会被采样捕获) */
void MDC1200_AppNoteOwnTx(void)
{
    MDC1200_AppDisableRx();
    s_ignore_next_self_rx = true;
    s_ignore_self_ticks = 1;      /* ~0.5 s — 仅覆盖TX尾残留 */
}

void MDC1200_AppTick10ms(void)
{
    if (!MDC1200_AppRxEnabled() || YAN_RF_ReceiveEnabled()) {
        if (s_sampling)
            mdc_sampling_stop();
        return;
    }

    if (gCurrentFunction == FUNCTION_TRANSMIT)
        return;    /* FUNCTION_Transmit 已调 AppDisableRx 停采样 */

    /* 门控: 静噪开(绿灯亮)时采样, 关时停止 (同GetLedState主路径) */
    bool gate = (g_SquelchLost || gCurrentFunction == FUNCTION_MONITOR);
#ifdef ENABLE_FMRADIO
    if (gFmRadioMode)
        gate = false;             /* BK1080 收音机模式不采 BK4819 音频 */
#endif

    if (gate) {
        if (!s_sampling)
            mdc_sampling_start();
        else
            mdc_drain_ring();
    } else {
        if (s_sampling) {
            mdc_drain_ring();            /* 收尾: 泄流残留字节 */
            mdc_sampling_stop();
            mdc_try_accept_rx();         /* 帧尾最终解码 */
            s_cap_len = 0;               /* 静噪关=帧彻底结束, 清缓冲 */
        }
    }
}

void MDC1200_AppTick500ms(void)
{
    if (gMdcId_RX_timeout > 0) {
        if (--gMdcId_RX_timeout == 0) {
            gMdcId_RX = 0;
            gMdcCallsign[0] = 0;
            gUpdateDisplay = true;
        }
    }
    if (s_ignore_next_self_rx && s_ignore_self_ticks > 0) {
        if (--s_ignore_self_ticks == 0)
            s_ignore_next_self_rx = false;
    }
}

/* 软接收采样激活查询 — 电池采样门控用
   (ADC在外部触发模式时BOARD_ADC_GetBatteryInfo的SW触发会死等EOS) */
bool MDC1200_AppRxSampling(void)
{
    return s_sampling;
}
