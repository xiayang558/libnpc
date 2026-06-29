/* ═══════════════════════════════════════════════════════════════
 *  I18N: Chinese translations for NumPy C Library docs
 *  Default language: Chinese (zh). Fallback: English (en).
 * ═══════════════════════════════════════════════════════════════ */

var I18N = {};
var LANG = (function() { try { return localStorage.getItem('npc_lang') || 'zh'; } catch(e) { return 'zh'; } })();

/* ── Module names & descriptions ── */
I18N.modules = {
  create:  { name:'数组创建', desc:'生成新 Array 对象的工厂函数。' },
  shape:   { name:'形状与操作', desc:'reshape、transpose、stack、split、broadcast 等维度变换。' },
  arith:   { name:'算术与数学', desc:'逐元素加减乘除、sqrt、exp、log、power、mod、hypot 等。' },
  trig:    { name:'三角函数', desc:'sin、cos、tan、反三角函数、双曲函数、角度弧度转换。' },
  fp:      { name:'浮点运算', desc:'fabs、copysign、nextafter、spacing、frexp、modf。' },
  stats:   { name:'统计', desc:'sum、mean、std、var、median、percentile、cumsum、cov、trapz 等。' },
  nanstats:{ name:'NaN 统计', desc:'nanmax、nanmin、nanmean、nanmedian、nanpercentile、nancumsum 等。' },
  linalg:  { name:'线性代数', desc:'matmul、dot、svd、qr、lu、cholesky、inv、pinv、det、trace 等。' },
  fft:     { name:'FFT 傅里叶变换', desc:'Cooley-Tukey 基-2 DIT。fft/ifft/fft2/fftn/rfft/irfft 及工具函数。' , notes:'长度必须为 2 的幂。纯实数输入：使用 rfft() 更高效（输出量减半）。', ret:'一维 COMPLEX128 数组，长度与输入相同', params:['Input (any dtype; converted to COMPLEX128). Length must be power of 2']},
  datetime:{ name:'日期时间', desc:'datetime64 类型：now、from_string、as_string、日期加减、比较、is_busday。' },
  random:  { name:'随机数', desc:'xoshiro256** 算法。44 种分布：normal、gamma、binomial 等。' },
  poly:    { name:'多项式', desc:'poly、roots、polyadd、polymul、polydiv、polyder、polyfit、polyval。' , notes:'p(x)=(x−r₁)(x−r₂)⋯(x−rₙ)=x^n+p[1]x^(n−1)+…+p[n]。首项系数恒为1。', ret:'一维FLOAT64数组，长度=len(roots)+1', params:['1-D array of roots (complex or real)']},
  window:  { name:'窗函数', desc:'hamming、hanning、bartlett、blackman、kaiser。' },
  interp:  { name:'插值', desc:'interp — 一维线性插值。' , ret:'FLOAT64 数组，与 x 形状相同', params:['Evaluation points (any shape, any real dtype)','1-D known x-coordinates (must be strictly increasing)','1-D known y-coordinates (same length as xp)','Fill value for x < xp[0] — if NULL, uses fp[0]','Fill value for x > xp[-1] — if NULL, uses fp[-1]']},
  signal:  { name:'信号处理', desc:'convolve、correlate。' },
  compare: { name:'比较与逻辑', desc:'greater、equal、less、isclose、logical_and/or/xor/not、all、any 等。' },
  searchm: { name:'搜索与集合', desc:'unique、intersect1d、setdiff1d、digitize、searchsorted、argmax/min 等。' },
  manip:   { name:'数组操作', desc:'clip、choose、compress、npc_delete、extract、insert、put、around 等。' },
  bitwise: { name:'位运算', desc:'bitwise_and/or/xor/not、invert、left_shift、right_shift。' },
  io:      { name:'输入输出', desc:'readtxt、genfromtxt、savetxt、savez (NPZ)、frombuffer。' },
  utils:   { name:'工具函数', desc:'astype、copy、nan_to_num、unwrap、real/imag、ix_、bincount、lcm、gcd 等。' },
};

/* ── Static pages ── */
I18N.pages = {
  install: `
<style>
.inst-step { display:flex; gap:16px; align-items:flex-start; margin:20px 0; padding:18px 20px; border:1px solid #e8e8f0; border-radius:10px; }
.inst-step .num { flex-shrink:0; width:36px; height:36px; border-radius:50%; background:#4f46e5; color:#fff; display:flex; align-items:center; justify-content:center; font-weight:700; font-size:16px; }
.inst-step h3 { margin:0 0 4px; font-size:15px; color:#333; }
.inst-step p { margin:0; color:#666; font-size:13px; }
.inst-step .extra { margin-top:8px; }
.plat-grid { display:grid; grid-template-columns:repeat(auto-fill, minmax(240px,1fr)); gap:12px; margin:12px 0 24px; }
.plat-card { border:1px solid #e8e8f0; border-radius:10px; padding:16px 18px; }
.plat-card h3 { font-size:14px; margin:0 0 8px; display:flex; align-items:center; gap:6px; }
.plat-card p { font-size:12px; color:#666; margin:3px 0; }
.plat-card code { font-size:12px; }
.plat-icon { font-size:20px; }
.req-card { display:flex; align-items:center; gap:12px; padding:12px 16px; border:1px solid #e8e8f0; border-radius:8px; margin:8px 0; font-size:13px; }
.req-card .r-icon { font-size:20px; flex-shrink:0; }
.req-card strong { display:block; color:#333; }
.req-card span { color:#888; font-size:12px; }
.cmd-block { background:#1e1e2e; color:#cdd6f4; border-radius:8px; padding:14px 18px; margin:12px 0; font-family:'SF Mono','Fira Code',monospace; font-size:13px; line-height:1.5; overflow-x:auto; white-space:pre-wrap; word-break:break-all; }
.cmd-block .cmt { color:#6c7086; }
</style>
<div class="inst-step"><div class="num">1</div><div>
  <h3>环境要求</h3>
  <p>编译前需要的所有内容。</p>
  <div class="extra">
    <div class="req-card"><span class="r-icon">🔧</span><div><strong>C99 编译器</strong><span>GCC 5.x+ / Clang 3.3+ / Apple Clang 15+ / MinGW-w64</span></div></div>
    <div class="req-card"><span class="r-icon">📐</span><div><strong>libm</strong><span>标准 C 数学库 — 所有系统自带，无需额外安装</span></div></div>
    <div class="req-card"><span class="r-icon">📦</span><div><strong>libzip</strong><span>仅用于 NPZ 文件读写。<code>brew install libzip</code> (macOS) · <code>apt install libzip-dev</code> (Linux)</span></div></div>
  </div>
</div></div>
<div class="inst-step"><div class="num">2</div><div>
  <h3>克隆并编译</h3>
  <p>下载源码，编译静态库和共享库。</p>
</div></div>
<div class="cmd-block"><span class="cmt"># 克隆仓库</span>
git clone https://github.com/xiayang558/libnpc.git
cd libnpc

<span class="cmt"># 编译静态库（默认目标）</span>
make          <span class="cmt"># → lib/libnpc.a (~875 KB)</span>

<span class="cmt"># 编译共享库</span>
make shared   <span class="cmt"># → lib/libnpc.so (Linux) / lib/libnpc.dylib (macOS)</span>

<span class="cmt"># 编译并运行全部 172 项测试</span>
make test     <span class="cmt"># 静态链接 — 通常 170+ 通过</span>
make test-shared  <span class="cmt"># 共享链接 — 验证 .so/.dylib 可用</span>

<span class="cmt"># 安装到系统（同时复制静态库 + 共享库 + 头文件）</span>
make install  <span class="cmt"># → /usr/local（可用 PREFIX=/custom/path 覆盖）</span></div>
<div class="inst-step"><div class="num">3</div><div>
  <h3>链接到你的项目</h3>
  <p>包含头文件后，链接静态库（<code>libnpc.a</code>）或共享库（<code>libnpc.so</code> / <code>libnpc.dylib</code>）。</p>
</div></div>
<div class="cmd-block"><span class="cmt"># 编译你的源文件</span>
gcc -I<b>path/to/libnpc/include</b> -c myfile.c

<span class="cmt"># 选项A：静态链接（独立可执行文件）</span>
gcc myfile.o <b>path/to/libnpc/lib/libnpc.a</b> -lm -lzip -o myapp

<span class="cmt"># 选项B：共享链接（更小的可执行文件，运行时需要 .so/.dylib）</span>
gcc myfile.o -L<b>path/to/libnpc/lib</b> -lnpc -lm -lzip \
    -Wl,-rpath,<b>path/to/libnpc/lib</b> -o myapp

<span class="cmt"># 如果已安装到 /usr/local：</span>
gcc myfile.o -lnpc -lm -lzip -o myapp     <span class="cmt"># 自动在标准路径找到 libnpc</span></div>
<div class="inst-step"><div class="num">4</div><div>
  <h3>支持的平台</h3>
  <p>已在以下环境中完成测试。</p>
</div></div>
<div class="plat-grid">
<div class="plat-card"><h3><span class="plat-icon">🍎</span> macOS</h3><p>架构：<strong>arm64</strong> (Apple Silicon) / <strong>x86_64</strong> (Intel)</p><p>编译器：<strong>Apple Clang 15+</strong></p><p>依赖：<code>brew install libzip</code></p></div>
<div class="plat-card"><h3><span class="plat-icon">🐧</span> Ubuntu</h3><p>架构：<strong>x86_64</strong> / <strong>aarch64</strong></p><p>编译器：<strong>GCC 5+</strong> 或 <strong>Clang 3.3+</strong></p><p>依赖：<code>sudo apt install libzip-dev</code></p></div>
<div class="plat-card"><h3><span class="plat-icon">🐲</span> Kylin</h3><p>架构：<strong>x86_64</strong> / <strong>aarch64</strong> (Phytium, Kunpeng)</p><p>编译器：<strong>GCC 5+</strong> 或 <strong>Clang 3.3+</strong></p><p>依赖：<code>sudo yum install libzip-devel</code></p></div>
<div class="plat-card"><h3><span class="plat-icon">🥧</span> Raspberry Pi</h3><p>架构：<strong>armhf</strong> (Pi 3/4) / <strong>aarch64</strong> (Pi 4/5, 64-bit OS)</p><p>编译器：<strong>GCC 5+</strong></p><p>依赖：<code>sudo apt install libzip-dev</code></p></div>
<div class="plat-card"><h3><span class="plat-icon">🪟</span> Windows</h3><p>架构：<strong>x86_64</strong></p><p>编译器：<strong>MinGW-w64</strong> (MSYS2)</p><p>依赖：<code>pacman -S libzip</code></p></div>
</div>
<h3>Makefile 目标</h3>
<style>
.mk-grid { display:grid; grid-template-columns:repeat(auto-fill, minmax(200px,1fr)); gap:10px; margin:12px 0 24px; }
.mk-card { border:1px solid #e8e8f0; border-radius:8px; padding:14px 16px; }
.mk-card code { font-weight:600; color:#4f46e5; font-size:13px; background:transparent; padding:0; }
.mk-card p { font-size:12px; color:#888; margin:4px 0 0; }
</style>
<div class="mk-grid">
<div class="mk-card"><code>make</code><p>编译 lib/libnpc.a — 默认目标</p></div>
<div class="mk-card"><code>make shared</code><p>编译 lib/libnpc.so / libnpc.dylib</p></div>
<div class="mk-card"><code>make test</code><p>编译并运行全部 172 项测试</p></div>
<div class="mk-card"><code>make runtest</code><p>运行测试（不重新编译）</p></div>
<div class="mk-card"><code>make demo</code><p>编译并运行示例程序</p></div>
<div class="mk-card"><code>make clean</code><p>删除 build/ build-pic/ lib/</p></div>
<div class="mk-card"><code>make install</code><p>安装到 /usr/local（可设 PREFIX=）</p></div>
</div>
`,

  datatypes: `
<style>
.dt-tbl { width:100%; border-collapse:collapse; border-radius:8px; overflow:hidden; box-shadow:0 1px 4px rgba(0,0,0,0.06); margin:12px 0 24px; }
.dt-tbl th { background:#4f46e5; color:#fff; padding:10px 14px; font-size:12px; text-align:left; font-weight:600; letter-spacing:.3px; }
.dt-tbl td { padding:9px 14px; border-bottom:1px solid #f0f0f4; font-size:13px; }
.dt-tbl tr:last-child td { border-bottom:none; }
.dt-tbl tbody tr:hover { background:#fafaff; }
.dt-tbl .dt-name { font-weight:600; color:#4f46e5; font-family:'SF Mono','Fira Code',monospace; font-size:13px; }
.dt-cat { display:inline-block; padding:2px 8px; border-radius:99px; font-size:10px; font-weight:600; letter-spacing:.3px; }
.cat-bool { background:#e0f2fe; color:#0369a1; }
.cat-int  { background:#dcfce7; color:#15803d; }
.cat-uint { background:#fef3c7; color:#a16207; }
.cat-flt  { background:#ede9fe; color:#6d28d9; }
.cat-cplx { background:#fce7f3; color:#be185d; }
.cat-spec { background:#f1f5f9; color:#475569; }
.dt-range { color:#999; font-size:11px; font-style:italic; }
.promo-grid { display:grid; grid-template-columns:repeat(auto-fill, minmax(260px,1fr)); gap:10px; margin:12px 0 24px; }
.promo-item { display:flex; align-items:center; gap:10px; padding:10px 14px; border:1px solid #e8e8f0; border-radius:8px; font-size:13px; }
.promo-item code { font-size:12px; }
.promo-arrow { color:#4f46e5; font-weight:700; }
.promo-reason { color:#888; font-size:11px; margin-left:auto; }
</style>
<p style="margin:0 0 4px;color:#666;font-size:14px;">17 种数据类型，分为六类。均在 <code>DataType</code> 枚举中定义。</p>
<table class="dt-tbl">
<thead><tr><th>枚举值</th><th>C 类型</th><th>大小</th><th>NumPy</th><th>取值范围</th></tr></thead>
<tbody>
<tr><td><span class="dt-cat cat-bool">BOOL</span></td><td><code>uint8_t</code></td><td>1 B</td><td>bool_</td><td class="dt-range">false (0) / true (1)</td></tr>
<tr><td><span class="dt-cat cat-int">INT8</span></td><td><code>int8_t</code></td><td>1 B</td><td>int8</td><td class="dt-range">−128 … 127</td></tr>
<tr><td><span class="dt-cat cat-int">INT16</span></td><td><code>int16_t</code></td><td>2 B</td><td>int16</td><td class="dt-range">−32,768 … 32,767</td></tr>
<tr><td><span class="dt-cat cat-int">INT32</span></td><td><code>int32_t</code></td><td>4 B</td><td>int32</td><td class="dt-range">−2.1×10⁹ … 2.1×10⁹</td></tr>
<tr><td><span class="dt-cat cat-int">INT64</span></td><td><code>int64_t</code></td><td>8 B</td><td>int64</td><td class="dt-range">−9.2×10¹⁸ … 9.2×10¹⁸</td></tr>
<tr><td><span class="dt-cat cat-uint">UINT8</span></td><td><code>uint8_t</code></td><td>1 B</td><td>uint8</td><td class="dt-range">0 … 255</td></tr>
<tr><td><span class="dt-cat cat-uint">UINT16</span></td><td><code>uint16_t</code></td><td>2 B</td><td>uint16</td><td class="dt-range">0 … 65,535</td></tr>
<tr><td><span class="dt-cat cat-uint">UINT32</span></td><td><code>uint32_t</code></td><td>4 B</td><td>uint32</td><td class="dt-range">0 … 4.3×10⁹</td></tr>
<tr><td><span class="dt-cat cat-uint">UINT64</span></td><td><code>uint64_t</code></td><td>8 B</td><td>uint64</td><td class="dt-range">0 … 1.8×10¹⁹</td></tr>
<tr><td><span class="dt-cat cat-flt">FLOAT32</span></td><td><code>float</code></td><td>4 B</td><td>float32</td><td class="dt-range">±1.5×10⁻⁴⁵ … 3.4×10³⁸</td></tr>
<tr><td><span class="dt-cat cat-flt">FLOAT64</span></td><td><code>double</code></td><td>8 B</td><td>float64</td><td class="dt-range">±5.0×⁻³²⁴ … 1.7×10³⁰⁸</td></tr>
<tr><td><span class="dt-cat cat-flt">FLOAT128</span></td><td><code>long double</code></td><td>16 B</td><td>float128</td><td class="dt-range">与平台相关</td></tr>
<tr><td><span class="dt-cat cat-cplx">COMPLEX64</span></td><td><code>complex float</code></td><td>8 B</td><td>complex64</td><td class="dt-range">2×FLOAT32 (实部 + 虚部)</td></tr>
<tr><td><span class="dt-cat cat-cplx">COMPLEX128</span></td><td><code>complex double</code></td><td>16 B</td><td>complex128</td><td class="dt-range">2×FLOAT64 (实部 + 虚部)</td></tr>
<tr><td><span class="dt-cat cat-cplx">COMPLEX256</span></td><td><code>complex long double</code></td><td>32 B</td><td>complex256</td><td class="dt-range">2×FLOAT128 (实部 + 虚部)</td></tr>
<tr><td><span class="dt-cat cat-spec">DATETIME64</span></td><td><code>int64_t</code> + unit enum</td><td>8 B</td><td>datetime64</td><td class="dt-range">纳秒 … 周级精度</td></tr>
<tr><td><span class="dt-cat cat-spec">STRING</span></td><td><code>char*</code></td><td>—</td><td>str_</td><td class="dt-range">以 null 结尾</td></tr>
</tbody></table>
<h3>类型提升规则</h3>
<p style="color:#666;font-size:14px;margin-bottom:10px;">二元运算通过 <code>promote_type(t1,t2)</code> 自动提升类型。规则如下：</p>
<div class="promo-grid">
<div class="promo-item"><code>INT+INT</code><span class="promo-arrow">→</span><code>INT64</code><span class="promo-reason">最宽的整数</span></div>
<div class="promo-item"><code>INT+FLOAT</code><span class="promo-arrow">→</span><code>FLOAT64</code><span class="promo-reason">浮点优先</span></div>
<div class="promo-item"><code>UINT+INT</code><span class="promo-arrow">→</span><code>INT64</code><span class="promo-reason">有符号优先</span></div>
<div class="promo-item"><code>FLOAT32+FLOAT64</code><span class="promo-arrow">→</span><code>FLOAT64</code><span class="promo-reason">最宽的浮点</span></div>
<div class="promo-item"><code>REAL+COMPLEX</code><span class="promo-arrow">→</span><code>COMPLEX</code><span class="promo-reason">复数优先</span></div>
<div class="promo-item"><code>BOOL+ANY</code><span class="promo-arrow">→</span><code style="font-size:12px">对方类型</code><span class="promo-reason">bool→0/1</span></div>
</div>
<h3>辅助函数</h3>
<pre><code>size_t      dtype_size(DataType d);     // 每元素字节数
const char* dtype_name(DataType d);     // "FLOAT64","INT32","COMPLEX128",…
int         is_integer(DataType d);     // INT/UINT/BOOL 返回 true
DataType    promote_type(DataType a, DataType b);</code></pre>
`,

  headers: `
<style>
.hdr-g { border:1px solid #e0e0e8; border-radius:8px; margin:12px 0; padding:14px 16px; }
.hdr-g h3 { font-size:13px; color:#4f46e5; font-family:SF Mono,Fira Code,monospace; margin:0 0 4px; }
.hdr-g p { font-size:12px; color:#666; margin:0; line-height:1.5; }
</style>
<p style="color:#666;font-size:13px;margin:0 0 12px;"><code>include/</code> 目录共 16 个头文件，定义了 libnpc 的全部公开 API。</p>
<div class="hdr-g"><h3>array.h</h3><p>核心头文件。<code>Array</code> 结构体、<code>DataType</code> 枚举，约 300 个函数声明。</p></div>
<div class="hdr-g"><h3>def.h</h3><p>常量定义：<code>TRUE</code>/<code>FALSE</code>、<code>INF</code>/<code>NAN</code>、<code>M_PI</code>、<code>M_EPSILON_F32</code>。</p></div>
<div class="hdr-g"><h3>random.h</h3><p>RNG 引擎 + 49 种概率分布。xoshiro256** 算法，splitmix64 种子初始化。</p></div>
<div class="hdr-g"><h3>shape.h</h3><p>reshape、transpose、flatten、stack/hstack/vstack、split、tile、flip 等形状操作。</p></div>
<div class="hdr-g"><h3>function.h</h3><p>lcm、gcd_array、aldexp、left_shift/right_shift、less/less_equal、linspace、logspace、readtxt 等。</p></div>
<div class="hdr-g"><h3>window.h</h3><p>bartlett、blackman、hamming、hanning、kaiser 窗函数。</p></div>
<div class="hdr-g"><h3>datetime64.h</h3><p>DateTime64 结构体 + DateTimeUnit 枚举。now、from_string、add/sub_days、is_busday 等。</p></div>
<div class="hdr-g"><h3>io.h</h3><p>save_npy、load_npy、save_npz、load_npz、save、load 等文件读写函数。</p></div>
<div class="hdr-g"><h3>linalg.h</h3><p>eig 特征值分解。</p></div>
<div class="hdr-g"><h3>einsum.h</h3><p>einsum、einsum_path Einstein 求和约定。</p></div>
<div class="hdr-g"><h3>histogram.h</h3><p>histogram、histogram2d、histogramdd 直方图函数。</p></div>
<div class="hdr-g"><h3>npzfile.h</h3><p>npzfile_open、npzfile_close、npzfile_keys、npzfile_get、bagobj_get 等 NPZ 归档操作。</p></div>
<div class="hdr-g"><h3>utils.h</h3><p>nanargmax/min、nan_to_num、isnan_array、isinf_array、isfinite_array、unique、msort 等工具函数。</p></div>
<div class="hdr-g"><h3>np_math_macros.h</h3><p>DEFINE_UNARY_FUNC 等宏定义、步长辅助函数、溢出检测。</p></div>
<div class="hdr-g"><h3>_dt_utils.h</h3><p>内部函数：unit_to_ns、is_holiday。</p></div>
  `,

  patterns: `
<style>
.pattern { border:1px solid #e8e8f0; border-radius:10px; margin:20px 0; overflow:hidden; }
.pattern-head { background:#fafaff; padding:14px 18px; border-bottom:1px solid #e8e8f0; display:flex; align-items:center; gap:10px; }
.pattern-head h3 { margin:0; font-size:15px; flex:1; }
.pattern-head .tag { font-size:10px; padding:2px 8px; border-radius:99px; background:#eef2ff; color:#4338ca; }
.pattern-body { padding:16px 18px; }
.pattern-body p { color:#555; font-size:13px; margin:0 0 10px; line-height:1.5; }
.pattern-body pre { margin:0; }
.result { background:#f0fdf4; border-left:3px solid #22c55e; padding:8px 14px; margin-top:10px; border-radius:0 6px 6px 0; font-size:13px; color:#15803d; }
.result code { background:transparent; font-size:12px; }
.section-intro { margin:28px 0 12px; padding-bottom:6px; border-bottom:2px solid #e8e8f0; }
.section-intro h2 { font-size:18px; color:#333; margin:0; }
.section-intro p { color:#888; font-size:13px; margin:4px 0 0; }
</style>

<p style="margin:0 0 16px;color:#555;font-size:14px;line-height:1.6;">实用示例菜谱，涵盖最常用的任务——从创建数组到线性代数和信号处理。每个示例都是独立的，可直接编译运行。</p>

<div class="section-intro"><h2>创建与查看数组</h2><p>基本构建块。</p></div>

<div class="pattern"><div class="pattern-head"><h3>从零开始创建数组</h3><span class="tag">create</span></div><div class="pattern-body">
<p>使用 <code>zeros</code> 预分配、<code>arange</code> 生成序列、<code>eye</code> 创建单位矩阵、<code>full</code> 常量填充。</p>
<pre><code>int s2[]  = {3, 4};
int s1[]  = {10};

Array *z  = zeros(s2, 2, FLOAT64);        // 3x4 的全 0.0，计算缓冲区
Array *a  = arange(1, 10, 1, FLOAT64);    // 1-D: [1, 2, ..., 9]
Array *I  = eye(3, 3, 0, FLOAT64);        // 3x3 单位矩阵
Array *f  = full(s2, 2, FLOAT64, &(double){3.14}); // 3x4 的全 3.14
Array *u  = empty(s2, 2, FLOAT64);        // 未初始化（更快，需自行填充）</code></pre>
<div class="result">💡 <code>zeros()</code> 调用 <code>calloc</code>；<code>empty()</code> 跳过初始化。仅在会覆盖所有元素时使用 <code>empty()</code>。</div>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>类型转换</h3><span class="tag">create</span></div><div class="pattern-body">
<p>使用 <code>astype</code> 将整数数据转换为浮点以进行除法，或将实数转复数。</p>
<pre><code>Array *ints   = arange(0, 10, 1, INT32);       // 0,1,...,9 (INT32)
Array *floats = astype(ints, FLOAT64);           // 0.0,1.0,...,9.0 (FLOAT64)
Array *cplx   = astype(floats, COMPLEX128);      // (0+0i),(1+0i),... (imag=0)
Array *realp  = astype(cplx, FLOAT64);           // 只保留实部</code></pre>
<div class="result">⚠️ 复数 → 实数 <b>只保留实部</b>。实数 → 复数设置 imag=0。</div>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>查看数组</h3><span class="tag">debug</span></div><div class="pattern-body">
<p><code>print_array</code> 显示值；<code>array_info</code> 打印形状+dtype；<code>asize</code> 获取轴长度。</p>
<pre><code>Array *A = ones((int[]){3,5}, 2, FLOAT64);
print_array(A);          // 格式化的嵌套输出
array_info(A);           // 打印：ndim、shape、dtype、总元素数
int nr = asize(A, 0);    // 3 (行数)
int nc = asize(A, 1);    // 5 (列数)
int tot = asize(A, -1);  // 15 (总元素数)</code></pre>
</div></div>

<div class="section-intro"><h2>改变数组形状</h2><p>reshape、stack、split 和 broadcast，无需复制数据。</p></div>

<div class="pattern"><div class="pattern-head"><h3>reshape、flatten、transpose</h3><span class="tag">shape</span></div><div class="pattern-body">
<p>三者均返回视图（不复制内存）。<code>reshape</code> 必须保持总元素数不变。</p>
<pre><code>Array *v = arange(1, 13, 1, FLOAT64);        // 12 个元素的一维数组
Array *M = reshape(v, (int[]){3, 4}, 2);      // 视图为 3x4（同一片数据！）
Array *T = transpose(M);                       // 视图为 4x3（仍是同一片数据）
Array *f = flatten(M);                         // 一维副本（新内存分配）</code></pre>
<div class="result">💡 <code>reshape</code> 和 <code>transpose</code> 是 O(1) 的。修改视图会修改原数组！</div>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>堆叠数组</h3><span class="tag">shape</span></div><div class="pattern-body">
<p>沿新轴（stack）或已有轴（concatenate）合并数组。</p>
<pre><code>Array *a = arange(1,4,1,FLOAT64), *b = arange(4,7,1,FLOAT64);
Array *vs = vstack((Array*[]){a,b}, 2);   // 2x3: 2 行
Array *hs = hstack((Array*[]){a,b}, 2);   // 1x6: 6 列
Array *st = stack((Array*[]){a,b}, 2, 0); // 2x3: 新轴在位置 0

int s[] = {2,3};
Array *X = ones(s,2,FLOAT64), *Y = zeros(s,2,FLOAT64);
Array *Z = concatenate(X, Y, 0);          // 4x3 行合并</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>广播 (Broadcasting)</h3><span class="tag">shape</span></div><div class="pattern-body">
<p>(3,4) 矩阵 + (4,) 向量：向量沿轴 0 隐式拉伸以匹配。适用于所有二元运算。</p>
<pre><code>int sA[] = {3, 4}, sB[] = {4};
Array *A = ones(sA, 2, FLOAT64);               // 3x4 的全 1.0
Array *B = arange(1, 5, 1, FLOAT64);           // [1, 2, 3, 4]

Array *C = add(A, B);         // B 被广播: (4,) → (1,4) → (3,4)，然后相加
// 结果：C 的每一行 = [2, 3, 4, 5]

Array *D = multiply(A, B);    // 同样广播：每行 = [1, 2, 3, 4]</code></pre>
<div class="result">广播规则（NumPy 兼容）：(1) 在较小的 ndim 前补 1，(2) 维度必须相等或其中之一为 1。</div>
</div></div>

<div class="section-intro"><h2>统计与聚合</h2><p>沿轴归约数组，计算描述性统计量。</p></div>

<div class="pattern"><div class="pattern-head"><h3>轴控制：keepdims 详解</h3><span class="tag">stats</span></div><div class="pattern-body">
<p><code>axis</code> 选择归约的维度。<code>keepdims</code> 将归约轴保持为 size-1 以便广播。</p>
<pre><code>Array *M = reshape(arange(1,25,1,FLOAT64), (int[]){4,6}, 2);  // 4x6

Array *colMean = mean(M, 0, 0);   // shape (6)  折叠行
Array *rowMean = mean(M, 1, 0);   // shape (4)  折叠列
Array *total   = mean(M, -1, 0);  // 标量 (ndim=0) 全局均值

Array *cmK = mean(M, 0, 1);       // shape (1,6) 可广播回去
Array *rmK = mean(M, 1, 1);       // shape (4,1) 用于列去均值</code></pre>
<div class="result">💡 <code>axis=-1</code> 表示先展平所有元素。<code>keepdims=1</code> 在需要广播回去做减法/去均值时必不可少。</div>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>描述性统计</h3><span class="tag">stats</span></div><div class="pattern-body">
<p>常用统计量一行代码搞定。全部返回 FLOAT64。</p>
<pre><code>Array *data = random_normal(0.0, 1.0, (int[]){1000}, 1);  // N(0,1)

Array *mu    = mean(data, -1, 0);    // 总体均值
Array *sigma = std(data, -1, 0);     // 总体标准差 (ddof=0)
Array *svar  = var(data, -1, 1, 0);  // 样本方差 (ddof=1)
Array *med   = median(data, -1, 0);  // 中位数（内部排序）
double  q95   = *(double*)percentile(data,
                 create_array((int[]){1},1,FLOAT64), -1)->data; // 95分位
Array *rng   = ptp(data, -1, 0);     // 极差 = max − min</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>累积与差分</h3><span class="tag">stats</span></div><div class="pattern-body">
<pre><code>Array *a  = arange(1, 7, 1, FLOAT64);    // [1,2,3,4,5,6]
Array *cs = cumsum(a, -1);                // [1,3,6,10,15,21]
Array *d1 = diff(a, 1, -1);               // [1,1,1,1,1] 一阶差分
Array *d2 = diff(a, 2, -1);               // [0,0,0,0] 二阶差分</code></pre>
</div></div>

<div class="section-intro"><h2>线性代数</h2><p>矩阵运算、分解和求解器。</p></div>

<div class="pattern"><div class="pattern-head"><h3>矩阵乘法</h3><span class="tag">linalg</span></div><div class="pattern-body">
<p><code>matmul</code> 用于标准二维和批量。<code>dot</code> 用于向量。<code>outer</code> 用于全对外积。</p>
<pre><code>int s[]={3,3};
Array *A = reshape(arange(1,10,1,FLOAT64),s,2), *B = eye(3,3,0,FLOAT64);
Array *C = matmul(A, B);        // A @ B  3x3

Array *v = arange(1,4,1,FLOAT64), *w = arange(4,7,1,FLOAT64);
double dp = *(double*)dot(v,w)->data;       // 1x4+2x5+3x6 = 32
Array *o = outer(v, w);                     // 3x3: v[i] * w[j]</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>求解 Ax = b</h3><span class="tag">linalg</span></div><div class="pattern-body">
<pre><code>int s[]={3,3}; double vals[] = {4,1,1, 1,3,1, 1,1,2};
Array *A = create_array(s,2,FLOAT64); memcpy(A->data,vals,9*sizeof(double));
Array *b = arange(1,4,1,FLOAT64);

Array *Ai = inv(A);                         // 逆矩阵
Array *x  = matmul(Ai, b);                  // 求解 A x = b

Array *Ap = pinv(A, 1e-12);                // 伪逆（支持任意形状）
Array *xs = matmul(Ap, b);                  // 最小二乘解</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>SVD 与条件数</h3><span class="tag">linalg</span></div><div class="pattern-body">
<pre><code>SVDResult r = svd(A, 1);              // 经济 SVD: A = U @ diag(s) @ Vt
double smax = *(double*)r.s->data;    // 最大奇异值
double smin = *(double*)(r.s->data + (r.s->size-1)*sizeof(double));
double cond = smax / smin;            // 条件数

Array *UTU = matmul(transpose(r.U), r.U); // 应该约等于单位矩阵
free_array(r.U); free_array(r.s); free_array(r.Vt);</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>Cholesky 分解</h3><span class="tag">linalg</span></div><div class="pattern-body">
<p>用于对称正定矩阵：A = L L^T。</p>
<pre><code>Array *L    = cholesky(A);                // 下三角
Array *LT   = transpose(L);
Array *check = matmul(L, LT);             // 应等于 A</code></pre>
</div></div>

<div class="section-intro"><h2>信号处理</h2><p>FFT、卷积、窗函数和相位解缠绕。</p></div>

<div class="pattern"><div class="pattern-head"><h3>FFT 往返验证</h3><span class="tag">fft</span></div><div class="pattern-body">
<p>正向 FFT 后反向 FFT 恢复原始信号（浮点误差内）。</p>
<pre><code>Array *sig   = arange(1, 9, 1, FLOAT64);  // 8 个实数样本
Array *spec  = fft(sig);                    // 8 个复数频率分量
Array *recon = ifft(spec);                  // 恢复 sig（误差约 1e-14）</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>实频谱分析</h3><span class="tag">fft</span></div><div class="pattern-body">
<p>对纯实数数据使用 <code>rfft</code>（更快，输出减半）。配合 <code>hanning</code> 窗减少频谱泄漏。</p>
<pre><code>int N = 1024; double dt = 0.001;              // 1 ms 采样间隔
Array *signal = random_normal(0, 1, (int[]){N}, 1);
Array *half   = rfft(signal);                  // N/2+1 = 513 个复数值
Array *freq   = rfftfreq(N, dt);               // [0, 0.98, 1.95, ..., 500] Hz

// 加 Hann 窗以获得更干净的频谱
Array *win    = hanning(N);
Array *wind   = multiply(signal, win);
Array *spec   = rfft(wind);                    // 加窗后的频谱
Array *mag    = abs_array(spec);               // 幅值</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>卷积</h3><span class="tag">signal</span></div><div class="pattern-body">
<p>result[k] = Σ a[i] · v[k-i]。等价于多项式乘法。</p>
<pre><code>Array *a = arange(1, 4, 1, FLOAT64);      // [1, 2, 3]
Array *b = arange(4, 7, 1, FLOAT64);      // [4, 5, 6]
Array *c = convolve(a, b);                 // [4, 13, 28, 27, 18]
// 4 = 1x4; 13 = 1x5+2x4; 28 = 1x6+2x5+3x4; ...</code></pre>
</div></div>

<div class="section-intro"><h2>随机数</h2><p>种子初始化、分布和抽样工具。</p></div>

<div class="pattern"><div class="pattern-head"><h3>基本随机生成</h3><span class="tag">random</span></div><div class="pattern-body">
<pre><code>random_seed(42);                                  // 确定性序列，便于调试
Array *u  = random_uniform(-1, 1, (int[]){500}, 1);    // [-1,1) 均匀分布
Array *n  = random_normal(0, 1, (int[]){500}, 1);      // 标准正态
Array *e  = random_exponential(1.5, (int[]){500}, 1);  // scale=1.5, mean=1.5
Array *g  = random_gamma(2.0, 3.0, (int[]){500}, 1);  // shape=2, scale=3</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>排列与抽样</h3><span class="tag">random</span></div><div class="pattern-body">
<p><code>shuffle</code> 原地洗牌；<code>permutation</code> 返回副本；<code>choice</code> 支持加权和无加权抽样。</p>
<pre><code>Array *deck = arange(1, 53, 1, FLOAT64);            // 1..52

random_shuffle(deck);                                // 原地洗牌

Array *hand = random_choice(deck, 5, 0, NULL);       // 5 张不重复抽取
Array *boot = random_choice(deck, 100, 1, NULL);     // 100 次有放回抽样

// 加权抽样
Array *probs = random_dirichlet(ones((int[]){10},1,FLOAT64), NULL, 1);
Array *wtd   = random_choice(deck, 10, 0, probs);    // 10 张按权重抽取</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>多元正态</h3><span class="tag">random</span></div><div class="pattern-body">
<p>从 N(μ, Σ) 中采样。内部使用 Cholesky 分解。</p>
<pre><code>Array *mu     = arange(0, 3, 1, FLOAT64);             // [0, 1, 2]
Array *Sigma  = eye(3, 3, 0, FLOAT64);                // 独立维度
// 添加相关性：ρ₁₂ = 0.5
((double*)Sigma->data)[1] = 0.5; ((double*)Sigma->data)[3] = 0.5;

Array *samps = random_multivariate_normal(mu, Sigma, (int[]){1000}, 1); // 1000x3</code></pre>
</div></div>

<div class="section-intro"><h2>多项式与拟合</h2><p>系数操作、求根和最小二乘拟合。</p></div>

<div class="pattern"><div class="pattern-head"><h3>求根</h3><span class="tag">poly</span></div><div class="pattern-body">
<p><code>roots</code> 通过伴随矩阵特征值（Francis QR）求根。<code>poly</code> 从根重建系数。</p>
<pre><code>// x^2 - 5x + 6 = (x-2)(x-3)
Array *coeff = create_array((int[]){3}, 1, FLOAT64);
double cvals[] = {1, -5, 6}; memcpy(coeff->data, cvals, 3*sizeof(double));
Array *r  = roots(coeff);          // [3+0i, 2+0i]
Array *p2 = poly(r);               // [1, -5, 6] 系数已重建</code></pre>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>多项式拟合</h3><span class="tag">poly</span></div><div class="pattern-body">
<p>对带噪声数据进行最小二乘拟合。底层使用 Vandermonde 矩阵 + 伪逆。</p>
<pre><code>Array *x    = linspace(-1, 1, 20);                     // 20 个训练点
Array *y    = sin_array(multiply(x, x));                // y = sin(x^2)
Array *c    = polyfit(x, y, 3);                         // 三次拟合系数

Array *xnew = linspace(-1, 1, 100);
Array *yhat = polyval(c, xnew);                         // 在新点上评估拟合曲线</code></pre>
</div></div>

<div class="section-intro"><h2>内存管理</h2><p>所有权规则，避免泄漏，安全清理模式。</p></div>

<div class="pattern"><div class="pattern-head"><h3>所有权规则</h3><span class="tag">memory</span></div><div class="pattern-body">
<p>每个工厂函数都会分配内存。每次分配必须恰好释放一次。视图共享原数组数据——不要释放两者。</p>
<pre><code>Array *A = zeros((int[]){100,100}, 2, FLOAT64);   // 分配数据缓冲区
Array *B = A;                                       // 同一指针
Array *C = reshape(A, (int[]){10000}, 1);           // 同一片数据的 VIEW
Array *D = copy(A);                                 // 独立副本

free_array(A);    // 释放 A、B、C 共享的数据缓冲区
free_array(D);    // OK——D 有自己的缓冲区
// B 和 C 现在已悬空——不要读取或释放它们</code></pre>
<div class="result">规则：每次分配对应一次 <code>free_array</code>，不是每个指针一次。视图（reshape、transpose、squeeze）不分配内存。</div>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>结构体结果的清理</h3><span class="tag">memory</span></div><div class="pattern-body">
<p>分解函数返回包含多个数组的结构体。逐个释放每个成员。</p>
<pre><code>SVDResult r = svd(A, 1);
// 使用 r.U、r.s、r.Vt
free_array(r.U); free_array(r.s); free_array(r.Vt);

LUResult lr = lu(A);
free_array(lr.P); free_array(lr.L); free_array(lr.U);

UniqueResult ur = unique(A, 1, 0, 1);
free_array(ur.values); free_array(ur.indices); free_array(ur.counts);</code></pre>
</div></div>

<div class="section-intro"><h2>扩展库</h2><p>如何添加你自己的函数。</p></div>

<div class="pattern"><div class="pattern-head"><h3>添加一元数学函数</h3><span class="tag">extend</span></div><div class="pattern-body">
<p>使用 <code>DEFINE_UNARY_FUNC</code> 宏。添加辅助函数，实例化一次，声明即可。</p>
<pre><code>// 第1步：在 include/np_math_macros.h 中添加实数辅助函数
static inline double  half_real(double x)          { return x * 0.5; }
static inline float   half_realf(float x)           { return x * 0.5f; }

// 第2步：在新的 .c 文件中实例化
DEFINE_UNARY_FUNC_NOCOMPLEX(half_array, half_real, half_realf, half_reall)

// 第3步：在 include/array.h 中声明
// Array* half_array(Array *arr);

// 第4步：将 .c 文件路径添加到 Makefile 的 SRC_DIRS 中</code></pre>
<div class="result"><code>DEFINE_UNARY_FUNC</code> 宏自动处理整数到浮点的提升、所有 dtype 分发、复数支持和错误检查。</div>
</div></div>

<div class="pattern"><div class="pattern-head"><h3>编写测试</h3><span class="tag">extend</span></div><div class="pattern-body">
<pre><code>#include "array.h"

int main() {
    Array *a = arange(1, 4, 1, FLOAT64);
    Array *r = half_array(a);               // 调用新函数
    printf("期望 [0.5, 1.0, 1.5]: "); print_array(r);

    free_array(a); free_array(r);
    return 0;
}</code></pre>
</div></div>
`,
};

/* ── Function descriptions (zh) ── */
I18N.fn = {
  // Array Creation
  arange:   { desc:'返回 [start, stop) 区间内等步长的值。类似 numpy.arange。', notes:'步长不能为零。当步长不能整除区间时，最后一个值可能小于 stop。内部使用 double 计算。' , ret:'指定类型的一维数组。长度 = ceil((stop-start)/step)', params:['区间起始值（包含）','区间结束值（不包含）','值之间的步长（不能为零）','输出数据类型——浮点用FLOAT64']},
  linspace: { desc:'返回 [start, stop] 区间内均匀分布的 num 个样本。类似 numpy.linspace。', notes:'返回类型总是 FLOAT32。若 num<2，只返回 stop。需要其他类型请用 astype() 转换。' , ret:'一维 FLOAT32 数组，长度为 num', params:['区间起始值（包含）','End of interval (inclusive)','样本数量（正确间距需≥2）']},
  zeros:    { desc:'返回指定形状和类型的全零数组。', notes:'内部使用 calloc 分配，所有字节为零。复数类型实部虚部均为 0。' , ret:'新分配的数组，填充为 0', params:['Array of dimension sizes (e.g. (int[]){3,4})','Number of dimensions (length of shape array)','Data type for elements — integer, float, complex, or BOOL']},
  ones:     { desc:'返回指定形状和类型的全 1 数组。' , notes:'复数类型虚部为 0。BOOL 类型的 ones 为 true(1)。', ret:'新数组，所有元素设为 1（或 1.0 / 1+0i / true）', params:['各维度大小','维度数','Data type — integer ones = 1, float ones = 1.0, complex ones = 1+0i, BOOL ones = true']},
  full:     { desc:'返回指定形状和类型、填充为给定标量值的数组。', notes:'value 指针必须指向与 dtype 对应的正确 C 类型变量。' , ret:'新数组，所有元素等于 *value', params:['各维度大小','维度数','Data type — must match the type of *value','Pointer to the fill value — cast to appropriate type (e.g. &myDouble)']},
  empty:    { desc:'返回未初始化的数组，内容未定义。类似 numpy.empty。', notes:'比 zeros() 更快，因为跳过了零填充。仅在会覆盖所有元素时使用。' , ret:'New Array with malloc', params:['各维度大小','维度数','Data type']},
  eye:      { desc:'返回二维数组，第 k 条对角线为 1，其余为 0。' , notes:'k=0 时类似单位矩阵（对角线全为1的方阵）。非方阵在超出的行/列上填 0。', ret:'N×M 数组', params:['行数','Number of columns','Diagonal offset — 0 = main diagonal, >0 = above main, <0 = below main','Data type for the array']},
  identity: { desc:'返回 n×n 单位矩阵。等价于 eye(n, n, 0, dtype)。' , notes:'eye(n, n, 0, dtype) 的简写形式。', ret:'n×n 数组', params:['方阵的大小','Data type']},
  diag:     { desc:'提取对角线（输入为二维时）或构造对角矩阵（输入为一维时）。' , notes:'输入为二维时，被提取的对角线长度为 min(rows, cols-|k|)。k>0 为超对角线，k<0 为次对角线。', ret:'若 v 为一维→方阵对角矩阵。若 v 为二维→一维对角线向量', params:['输入——1D构建对角矩阵；2D提取第k条对角线','Diagonal offset — 0 = main, >0 = above, <0 = below']},
  astype:   { desc:'将数组元素转换为新的数据类型。类似 numpy.ndarray.astype。', notes:'复数转实数取实部；实数转复数虚部为 0；浮点转整数截断。' , ret:'新数组，dtype 为新类型，形状不变', params:['源数组','Target data type — any valid DataType enum value']},
  copy:     { desc:'返回数组的深拷贝（新分配内存）。类似 numpy.copy 或 numpy.ndarray.copy。' , notes:'即使 arr 是视图，也会复制全部数据。步长不会被复制——结果总是连续的。', ret:'新数组，形状/dtype/数据完全一致', params:['输入数组']},
  diagflat: { desc:'将输入展平后作为对角线构建二维数组。类似 numpy.diagflat。' , ret:'方阵二维数组，大小 = |k| + len(展平后的 arr)', params:['输入数组——先展平再放在对角线上','Diagonal offset']},
  frombuffer:{ desc:'从原始内存缓冲区构造数组。不复制数据。类似 numpy.frombuffer。' , notes:'返回的数组是 VIEW。调用者必须保持原始缓冲区存活。不复制数据。', ret:'数组视图（is_view=1），共享缓冲区内存', params:['指向原始内存的指针','Interpretation type for the bytes','Number of elements to interpret','Byte offset into buffer before first element']},
  fromfunction:{ desc:'通过对每个索引调用函数来构造数组。类似 numpy.fromfunction。' , ret:'通过在每个坐标调用 func 填充的数组', params:['回调函数：接收索引数组和ndim，返回该坐标的值','Output array shape','维度数','Output data type']},
  full_like:{ desc:'返回与输入数组形状相同、填充为指定值的数组。类似 numpy.full_like。' , ret:'新数组，与 arr 形状相同，dtype 按指定', params:['模板数组——复制其shape和ndim','Pointer to fill value','Output data type (may differ from arr->dtype)']},
  geomspace:{ desc:'在对数刻度上返回等间距的数（几何级数）。类似 numpy.geomspace。' , notes:'生成 exp(linspace(log(start), log(stop), num)) 序列。start 和 stop 必须同号。endpoint=0 时最后一个值为 stop。', ret:'一维 FLOAT64 数组，长度为 num', params:['序列的第一个值','序列的最后一个值（endpoint=1时）','样本数量','非零则包含stop；0则不包含']},
  logspace: { desc:'在对数刻度上返回等间距的数。类似 numpy.logspace。' , notes:'底数为 10 时参数示例：start=0,stop=2,num=3 → [1,10,100]。数值为 base^(linspace(start,stop,num))。endpoint=0 时最后一个值为 base^stop。', ret:'一维 FLOAT64 数组，长度为 num', params:['第一个值的指数：base^start','最后一个值的指数：base^stop','样本数量','对数的底（10.0表示以10为底）','非零则包含stop；0则不包含']},
  meshgrid: { desc:'从坐标向量返回网格矩阵。支持 xy 和 ij 索引。类似 numpy.meshgrid。' , notes:'与 numpy 不同，此函数直接修改输入数组。调用后输入数组变为已广播的网格。', ret:'返回第一个数组指针；所有输入原地变形为可广播的 N 维形式', params:['一维输入数组的数组（原地修改为N维网格）','Number of input arrays','"xy" = Cartesian (first dim varies slowest); "ij" = matrix (first dim varies fastest)']},
  tri:      { desc:'返回在给定对角线及以下为1、以上为0的数组。类似 numpy.tri。' , ret:'N×M 数组', params:['行数','Number of columns','Diagonal offset (default 0 for main diagonal and below)','Data type']},
  vander:   { desc:'生成 Vandermonde 矩阵。列是输入向量的幂。类似 numpy.vander。' , notes:'increasing=0 时第 j 列为 x[i]^(N-1-j)。increasing=1 时第 j 列为 x[i]^j。', ret:'len(x)×N 的 FLOAT64 矩阵', params:['一维输入值','Number of columns; if 0 or > len(x), uses len(x)','0 = decreasing powers (x^(N-1) … x^0, numpy default); 1 = increasing (x^0 … x^(N-1))']},

  // Shape
  reshape:  { desc:'改变数组形状而不复制数据。总元素数必须匹配。', notes:'返回 VIEW——修改结果会修改原数组。' , ret:'新形状的视图（同一数据指针）。总大小不匹配时返回 NULL', params:['输入数组','Array of new dimension sizes','New number of dimensions']},
  transpose:{ desc:'反转所有轴。对于二维数组即矩阵转置。', notes:'返回 VIEW，数据与原数组共享。复共轭转置请用 conj_transpose()。' , ret:'所有轴反转的视图（shape[i]→output.shape[ndim-1-i]）', params:['任意维度的输入数组']},
  flatten:  { desc:'返回一维副本（总是分配新内存）。' , notes:'与 ravel 不同（可能返回视图），flatten 总是复制。可安全修改结果而不影响原数组。', ret:'一维数组（拷贝），相同 dtype，长度=arr->size', params:['任意形状的输入数组']},
  squeeze:  { desc:'移除长度为 1 的维度。axis=-1 移除所有长度为 1 的维度。' , notes:'如果没有长度为1的维度，原样返回数组（视图）。不能压缩非1维。', ret:'长度为 1 的维度被移除的视图', params:['输入数组','Specific axis to squeeze, or -1 to remove ALL length-1 dimensions']},
  tile:     { desc:'沿每个维度重复数组 reps[i] 次构造新数组。类似 numpy.tile。' , ret:'新数组，形状为 arr->shape[i]*reps[i]', params:['输入数组','Repetition factors, one per dimension of arr','Length of reps (must equal arr->ndim)']},
  pad:      { desc:'沿每个维度填充数组。模式：constant/edge/reflect/symmetric/wrap。' , ret:'填充后的数组，与arr相同dtype', params:['输入数组','1D INT64 array: (before_0, after_0, before_1, after_1, …) — length 2*ndim','"constant" (fill value), "edge" (repeat edge), "reflect", "symmetric", "wrap"','Fill value for "constant" mode (ignored for other modes)']},
  atleast_1d:{ desc:'将输入转换为至少一维的数组。标量→形状为(1,)的一维数组。' , ret:'ndim>=1的数组', params:['输入']},
  atleast_2d:{ desc:'将输入转换为至少二维的数组。一维→形状为(1,n)的二维数组。' , params:['输入']},
  atleast_3d:{ desc:'将输入转换为至少三维的数组。' , params:['输入']},
  block:{ desc:'从嵌套的块列表组装多维数组。类似 numpy.block。' , params:['行优先的rowsxcols数组列表','Number of block rows','Number of block columns']},
  broadcast_arrays:{ desc:'将多个数组广播至相同形状，返回新数组列表。' , params:['输入数组','Count','Output: common ndim','Output: common shape']},
  broadcast_to:{ desc:'在不复制数据的情况下，将数组广播到新的形状。' , notes:'返回 VIEW——数据不复制。如需独立内存请使用 copy()。', ret:'视图（共享数据）', params:['输入数组','Target shape (must be broadcastable)','Number of target dimensions']},
  column_stack:{ desc:'将一维数组作为列堆叠成二维数组。类似 numpy.column_stack。' , params:['要堆叠的一维数组','Count']},
  concatenate:{ desc:'沿现有轴连接两个数组。类似 numpy.concatenate。' , ret:'沿指定轴连接后的新数组', params:['第一个数组','Second array (must match arr1 on all dims except axis)','Existing axis to concatenate along']},
  dsplit:{ desc:'沿第三维（深度）拆分数组。类似 numpy.dsplit。' , params:['输入（>=3维）','Number','Output count']},
  dstack:{ desc:'沿第三维（深度）堆叠数组。类似 numpy.dstack。' , params:['要堆叠的数组','Count']},
  expand_dims:{ desc:'在指定位置插入一个长度为1的新维度。类似 numpy.expand_dims。' , ret:'增加一个大小为 1 的新维度的视图', params:['输入数组','Position for the new axis (0..ndim, where ndim appends at end)']},
  flip:{ desc:'沿指定轴反转数组元素顺序。类似 numpy.flip。' , params:['输入数组','Axis to reverse']},
  fliplr:{ desc:'左右翻转二维数组（axis=1）。类似 numpy.fliplr。' , params:['输入（>=2维）']},
  flipud:{ desc:'上下翻转二维数组（axis=0）。类似 numpy.flipud。' , params:['输入（>=2维）']},
  hsplit:{ desc:'沿第二维（列）均匀拆分数组。类似 numpy.hsplit。' , params:['输入','Number','Output count']},
  hstack:{ desc:'沿第二维水平堆叠数组。类似 numpy.hstack。' , params:['数组指针的数组','Number of arrays to stack']},
  kron:{ desc:'计算两个数组的 Kronecker 积。类似 numpy.kron。' , ret:'数组，形状=a.shape*b.shape（各维度元素级乘积）', params:['第一个因子','Second factor']},
  ravel:{ desc:'返回展平的一维数组。支持 C 序（行优先）和 Fortran 序（列优先）。' , notes:'C 序且数组连续→返回视图。F 序→分配副本。', ret:'一维数组', params:['输入数组','"C" (row-major, default) or "F" (column-major)']},
  repeat:{ desc:'沿指定轴重复数组的每个元素。类似 numpy.repeat。' , ret:'新数组，元素沿轴重复', params:['输入数组','INT64 array of repetition counts — must be broadcastable to arr.shape[axis]','Axis along which to repeat values; -1 = flatten first']},
  resize:{ desc:'改变数组形状，必要时截断或重复填充数据。' , notes:'新大小大于原始大小→元素循环重复填充。新大小小于原始→截断。', ret:'新数组（始终为拷贝），具有请求的形状', params:['输入（resize前先展平）','New dimension sizes','New number of dimensions']},
  roll:{ desc:'沿指定轴滚动数组元素。元素从尾部绕回到头部。' , params:['输入数组','Number of positions to shift (positive=right/down, negative=left/up)','Axis to roll along; -1 = flatten first, roll, then reshape']},
  rot90:{ desc:'在指定平面内逆时针旋转数组 90 度 k 次。类似 numpy.rot90。' , params:['输入（至少二维）','Number of 90° rotations (1..3 typical; 4 = identity)','First axis of the rotation plane (default 0)','Second axis of the rotation plane (default 1)']},
  split:{ desc:'沿指定轴将数组拆分为多个子数组。类似 numpy.split。' , notes:'数组沿轴必须能被均匀整除。', ret:'Array*指针数组，调用者需逐个释放各子数组及外层数组', params:['输入数组','Number of equal-sized sections','Axis to split along','Output: number of result arrays']},
  stack:{ desc:'沿新轴连接数组序列。类似 numpy.stack。' , notes:'输入形状(s0,s1,…)→输出形状在 axis 处插入新维度：(s0,…,s(axis-1),num_arrays,s_axis,…)。', ret:'比输入多一个维度的数组', params:['要堆叠的数组指针数组','Number of arrays to stack','Position of the new axis in the result']},
  take:{ desc:'沿指定轴按索引取元素。类似 numpy.take。' , ret:'沿 axis 为 indices->shape、其余为 arr->shape 的数组', params:['源数组','INT64 index array (any shape; flattened first)','Axis to index into']},
  vsplit:{ desc:'沿第一维（行）均匀拆分数组。类似 numpy.vsplit。' , params:['输入','Number','Output count']},
  vstack:{ desc:'沿第一维垂直堆叠数组。类似 numpy.vstack。' , params:['要堆叠的数组','Count']},

  // Trigonometry
  angle:{ desc:'计算复数数组中每个元素的辐角（相角）。可选输出角度制。类似 numpy.angle。' , ret:'FLOAT64数组，与输入同形状，范围为(-pi,pi]或(-180,180]', params:['复数数组','0→radians, 1→degrees']},
  arccos:{ desc:'逐元素反余弦 arccos(x)，返回值在 [0, π] 弧度内。' , notes:'输入超出 [-1,1] 产生 NaN。', params:['输入（任意数值类型）']},
  arccosh:{ desc:'逐元素反双曲余弦 arccosh(x)，x ≥ 1。' , notes:'x<1 产生 NaN。', params:['输入']},
  arcsin:{ desc:'逐元素反正弦 arcsin(x)，返回值在 [-π/2, π/2] 弧度内。' , notes:'输入超出 [-1,1] 产生 NaN。', ret:'整数=FLOAT64，浮点=保持原浮点', params:['输入']},
  arcsinh:{ desc:'逐元素反双曲正弦 arcsinh(x)。' , params:['输入']},
  arctan:{ desc:'逐元素反正切 arctan(x)，返回值在 [-π/2, π/2] 弧度内。' , params:['输入']},
  arctan2:{ desc:'逐元素双参数反正切 arctan2(y, x)，正确识别象限。类似 numpy.arctan2。' , notes:'atan2(0,0)→0（符合 IEEE 标准）。正确处理 x=0 的象限判定。', ret:'FLOAT64广播形状数组，范围为(-pi, pi]', params:['y坐标','x-coordinates']},
  arctanh:{ desc:'逐元素反双曲正切 arctanh(x)，|x| < 1。' , notes:'|x|≥1 产生 NaN。', params:['输入']},
  cos_array:{ desc:'逐元素余弦 cos(x)，输入单位为弧度。' , params:['输入（弧度）']},
  cosh_array:{ desc:'逐元素双曲余弦 cosh(x)。' , params:['输入']},
  deg2rad:{ desc:'角度转弧度。等价于 radians()。' , params:['输入（角度）']},
  degrees:{ desc:'弧度转角度。等价于 rad2deg()。' , params:['输入（弧度）']},
  rad2deg:{ desc:'弧度转角度。等价于 degrees()。' , params:['输入（弧度）']},
  radians:{ desc:'角度转弧度。等价于 deg2rad()。' , params:['输入（角度）']},
  sin_array:{ desc:'逐元素正弦 sin(x)，输入单位为弧度。' , ret:'整数=FLOAT64，浮点/复数=保持原类型', params:['输入（弧度）']},
  sinh_array:{ desc:'逐元素双曲正弦 sinh(x)。' , params:['输入']},
  tan_array:{ desc:'逐元素正切 tan(x)，输入单位为弧度。' , params:['输入（弧度）']},
  tanh_array:{ desc:'逐元素双曲正切 tanh(x)。' , params:['输入']},

  // Arithmetic
  add:      { desc:'逐元素加法，支持广播。类似 numpy.add。', notes:'支持整数溢出检测。对于标量加法，使用 add_scalar。' , ret:'广播形状的数组，dtype为x1和x2的提升类型', params:['第一个操作数','Second operand']},
  subtract: { desc:'逐元素减法，支持广播。' , ret:'广播形状的数组，自动提升数据类型', params:['被减数','Subtrahend']},
  multiply: { desc:'逐元素乘法，支持广播。' , ret:'广播形状的数组，自动提升数据类型', params:['第一个因子','Second factor']},
  divide:   { desc:'逐元素除法。整数除法向零截断。', notes:'除零时返回 NULL 并输出错误到 stderr。需要确保浮点输出请用 true_divide。' , ret:'广播形状的数组，dtype与输入相同', params:['被除数','Divisor']},
  power:    { desc:'逐元素幂运算 base^exp，支持广播。', notes:'两个数组都转为 double。负底数+非整数指数→NaN。0^0→1.0。' , ret:'FLOAT64 类型广播形状的结果数组', params:['基准值（转为double）','Exponent values (converted to double)']},
  mod:      { desc:'逐元素取模。结果符号与除数一致（Python/NumPy 行为）。', notes:'不支持复数。除零→结果为 0（无错误）。使用 fmod+符号修正。' , ret:'FLOAT64 类型广播形状的结果数组', params:['被除数（仅实数）','Divisor (real only)']},
  sqrt_array:{ desc:'逐元素平方根 √x。' , notes:'负数实数输入→返回 NULL 并输出错误到 stderr。负复数→产生复数结果。', ret:'整数=FLOAT32，浮点=保持原浮点类型，复数=保持原复数类型', params:['输入（任意数值dtype）']},
  exp_array: { desc:'逐元素指数函数 e^x。' , ret:'整数=FLOAT32，浮点/复数=保持原类型', params:['输入']},
  log_array: { desc:'逐元素自然对数 ln(x)。' , notes:'非正实数输入→返回 NULL 并输出错误到 stderr。', ret:'整数=FLOAT64，浮点=FLOAT64，复数=COMPLEX128', params:['输入（实数或复数）']},
  abs_array: { desc:'逐元素绝对值（实数）/ 模长（复数）。' , ret:'实数=保持原类型，复数=实部（COMPLEX64=FLOAT32，COMPLEX128=FLOAT64）', params:['输入（任意数值dtype）']},
  sign:     { desc:'逐元素符号指示：-1 / 0 / +1。NaN 保持 NaN。' , notes:'无符号类型：只有 0 或 1。浮点类型的 NaN 保持 NaN。', ret:'与输入相同的数据类型', params:['输入（BOOL/int/uint/float；不支持复数）']},
  logaddexp:{ desc:'log(exp(x1)+exp(x2))，使用 log1p 保证数值稳定性。' , notes:'正确处理 -∞：logaddexp(-∞, x) = x。', ret:'FLOAT64 类型广播形状的结果数组', params:['第一个数组（实数）','Second array (real)']},
  add_scalar:{ desc:'数组逐元素加标量。' },
  ahypot:{ desc:'逐元素计算 sqrt(x1² + x2²)（欧氏范数）。类似 numpy.hypot。' , notes:'使用 libm hypot() 保证数值稳定性（避免中间平方运算溢出）。', ret:'FLOAT64 类型广播形状的结果数组', params:['第一个数组（实数）','Second array (real)']},
  arint:{ desc:'四舍五入到最近整数，.5 时向偶数取整（银行家舍入）。类似 numpy.rint。' , params:['输入（实数）']},
  atrunc:{ desc:'截断小数部分，向零取整。类似 numpy.trunc。' , params:['输入（实数）']},
  ceil_array:{ desc:'向上取整，返回不小于输入的最小整数。类似 numpy.ceil。' , params:['输入（实数）']},
  floor_array:{ desc:'向下取整，返回不大于输入的最大整数。类似 numpy.floor。' , ret:'整数=FLOAT64，浮点=保持原浮点类型', params:['输入（实数）']},
  heaviside:{ desc:'Heaviside 阶跃函数。x<0 返回 0，x=0 返回 h，x>0 返回 1。' , ret:'FLOAT64 数组，与 x 形状相同', params:['输入（实数，任意dtype）','Value to return when x == 0 (commonly 0, 0.5, or 1)']},
  log10_array:{ desc:'逐元素常用对数 log10(x)。' },
  log2_array:{ desc:'逐元素以 2 为底的对数 log2(x)。' , params:['输入（实数或复数）']},
  logaddexp2:{ desc:'以 2 为底计算 log2(2^x1 + 2^x2)，避免溢出。' , ret:'FLOAT64 类型的结果数组', params:['第一个','Second']},
  mod_scalar:{ desc:'数组逐元素对标量取模。' , notes:'除数为0→结果用0填充。不支持复数。结果符号与除数一致。', ret:'FLOAT64数组，与arr形状相同', params:['被除数数组（实数，任意形状）','Pointer to divisor value (type must match arr->dtype)']},
  multiply_scalar:{ desc:'数组逐元素乘标量。' },
  sinc:{ desc:'归一化 sinc 函数：sin(πx)/(πx)。x=0 时返回 1。类似 numpy.sinc。' , ret:'大多数类型为FLOAT64，FLOAT32输入则为FLOAT32', params:['输入（实数）']},
  true_divide:{ desc:'强制浮点除法。整数输入也返回 FLOAT64。类似 numpy.true_divide。' , ret:'FLOAT64 类型广播形状的结果数组', params:['被除数','Divisor']},
  npc_ldexp:{ desc:'逐元素计算 x * 2^exp。类似 numpy.ldexp。' },
  reciprocal:{ desc:'逐元素计算 1/x。类似 numpy.reciprocal。' },
  positive:{ desc:'返回 +x（数值拷贝）。类似 numpy.positive。' },
  negative:{ desc:'逐元素计算 -x。类似 numpy.negative。' },
  fmod_arr:{ desc:'逐元素 C 风格取余（向零截断）。类似 numpy.fmod。' },
  npc_cbrt:{ desc:'逐元素计算立方根。类似 numpy.cbrt。' },
  square:{ desc:'逐元素计算 x^2。类似 numpy.square。' },
  absolute:{ desc:'逐元素绝对值。类似 numpy.absolute。' },
  pow_arr:{ desc:'逐元素幂运算 base^exp。类似 numpy.power。' },
  modf_arr:{ desc:'将浮点数拆分为小数和整数部分。类似 numpy.modf。' },

  // Statistics
  sum:      { desc:'沿指定轴求和。axis=-1 展开所有元素。', notes:'整数晋升为 INT64，浮点/复数保持原类型。' , ret:'在指定轴上归约/连接的结果数组', params:['输入数组','Axis to reduce (-1 = flatten all elements)','If non-zero, reduced axis kept as size 1']},
  mean:     { desc:'沿指定轴的算术平均值。结果总是 FLOAT64。' , notes:'均值 = 总和 / 计数。整数输入在求均值前转为 double。', ret:'FLOAT64 类型的结果数组', params:['输入','Axis (-1=flat)','Keep dims']},
  std:      { desc:'沿指定轴的标准差（ddof=0）。结果总是 FLOAT64。' , notes:'使用两遍算法：先求均值，再计算方差。ddof>0 请用 var() 参考。', ret:'FLOAT64 类型的结果数组', params:['输入','Axis (-1=flat)','Keep dims']},
  var:      { desc:'沿指定轴的方差，可配置 ddof。ddof=0 总体方差 (1/N)，ddof=1 样本方差 (1/(N-1))。' , ret:'FLOAT64 类型的结果数组', params:['输入','Axis (-1=flat)','Delta degrees of freedom: 0 = population variance (1/N), 1 = sample variance (1/(N-1))','Keep reduced dims']},
  median:   { desc:'沿指定轴的中位数。对数据副本排序。' , notes:'对数据副本排序。偶数长度轴→返回中间两值的均值。', ret:'FLOAT64 类型的结果数组', params:['输入（实数）','Axis (-1=flat)','Keep dims']},
  percentile:{ desc:'第 q 百分位数，q∈[0,100]。使用线性插值。' , notes:'线性插值：设 N=轴长度，p=百分位/100。索引=(N-1)*p。索引非整数时在相邻值间线性插值。', ret:'FLOAT64数组。若q为标量=移除轴后的形状；若q为向量=第一维=len(q)', params:['输入（实数）','1D FLOAT64 array of percentiles (0 to 100)','Axis (-1=flat)']},
  quantile: { desc:'第 q 分位数，q∈[0,1]。算法与 percentile 相同。' , params:['输入（实数）','1D FLOAT64 array of quantiles (0 to 1)','Axis (-1=flat)']},
  cumsum:   { desc:'沿指定轴的累积和。axis=-1 先展平。' , ret:'与输入相同的形状', params:['输入','Axis (-1=flat first)']},
  cumprod:  { desc:'沿指定轴的累积乘积。' , params:['输入','Axis (-1=flat)']},
  cov:      { desc:'协方差矩阵。rowvar=1 行为变量，rowvar=0 列为变量。' , ret:'FLOAT64协方差矩阵（方阵，大小=变量数）', params:['输入（1D=单变量；2D=多变量）','If non-zero, rows = variables; if 0, columns = variables','Delta degrees of freedom (0=biased, 1=unbiased)','Output data type']},
  corrcoef: { desc:'Pearson 相关系数矩阵。' , ret:'FLOAT64相关系数矩阵（对角线为1，非对角线-1到1）', params:['输入','If 1, rows=variables; if 0, columns=variables']},

  // Linear Algebra
  matmul:   { desc:'矩阵乘法，完整 NumPy 广播语义。支持批量维度和标量规则。' , notes:'arr1 最后一个轴与 arr2 倒数第二个轴收缩。支持批量矩阵乘法（前面维度广播）。', ret:'数组。规则：1D+1D=标量（点积）；1D+2D=1D；2D+2D=2D；>2D=批量矩阵乘法', params:['第一个操作数（任意维数）','Second operand (any ndim)']},
  dot:      { desc:'点积。二维同 matmul，一维同内积，多维收缩最后轴。' , notes:'与 matmul 的区别：对 >2D 张量，dot 收缩 a 最后轴与 b 倒数第二轴（不同于 matmul）。', ret:'一维=标量点积；二维=矩阵乘法；N维=张量收缩', params:['第一个','Second']},
  svd:      { desc:'奇异值分解。A = U @ diag(s) @ Vt。', notes:'纯 C Golub-Reinsch 算法。大矩阵请用 -DUSE_LAPACK 编译。' , ret:'SVDResult 结构体，包含 U、s、Vt 成员', params:['mxn实数矩阵（二维，FLOAT64）','1 = full U (m×m); 0 = economy U (m×k where k=min(m,n))']},
  qr:       { desc:'QR 分解。A = Q·R，Q 正交，R 上三角。' , notes:'Q^T·Q=I。R 为 n×n 上三角。纯 C 实现。', ret:'QRResult 结构体，包含 Q 和 R 成员', params:['mxn矩阵']},
  lu:       { desc:'LU 分解（含部分主元选取）。P·A = L·U。' , notes:'L 对角线全为 1。P 为行置换矩阵。纯 C 部分（行）主元选取。', ret:'LUResult 结构体，包含 P、L、U 成员', params:['nxn方阵']},
  cholesky: { desc:'Cholesky 分解。A = L·L^T，L 为下三角阵。', notes:'仅处理下三角部分。不验证正定性——调用者需自行保证。' , ret:'nxn下三角L', params:['nxn对称正定矩阵']},
  inv:      { desc:'矩阵求逆（通过 LU 分解）。奇异矩阵返回 NULL。' , notes:'通过 LU 分解+n 组三角求解计算逆矩阵。输入必须为方阵且可逆。', ret:'nxn逆矩阵，奇异时返回NULL', params:['nxn方阵']},
  pinv:     { desc:'Moore-Penrose 伪逆（通过 SVD）。支持矩形和秩亏矩阵。' , notes:'适用于秩亏和非方阵。默认 rcond≈1e-15。', ret:'nxm伪逆矩阵', params:['mxn矩阵（矩形即可）','Cutoff threshold: singular values < rcond * max_sv are truncated to zero']},
  det:      { desc:'矩阵行列式（通过 LU 分解）。' , notes:'det=U 对角线乘积×(-1)^(行交换次数)。奇异矩阵→0。', ret:'标量（ndim=0）FLOAT64数组', params:['nxn方阵']},
  trace:    { desc:'主对角线元素之和。' , ret:'标量FLOAT64数组', params:['二维数组']},
  cross:    { desc:'向量叉积。两输入必须是一维且长度为 3。' , ret:'长度为3的一维数组：c[0]=a[1]b[2]-a[2]b[1], c[1]=a[2]b[0]-a[0]b[2], c[2]=a[0]b[1]-a[1]b[0]', params:['第一个（必须为1-D，大小为3）','Second (must be 1-D, size 3)']},
  outer:    { desc:'外积。out[i,j] = a[i] * b[j]（先展平两输入）。' , ret:'二维数组，形状为(len(a), len(b))', params:['第一个（展平）','Second (flattened)']},

  // FFT
  fft:      { desc:'一维复数 FFT（Cooley-Tukey 基-2 DIT）。长度必须是 2 的幂。' },
  ifft:     { desc:'一维逆 FFT。结果除以 N（与 NumPy 一致）。' , notes:'fft→ifft 精确往返（浮点误差范围内）。', ret:'一维 COMPLEX128 数组，长度与输入相同', params:['COMPLEX128频谱']},
  rfft:     { desc:'实数一维 FFT。只返回非负频率（一半+1 个值）。利用共轭对称性，比 fft() 快约一倍。' , notes:'利用共轭对称性（X[-k]=conj(X[k])）避免计算冗余频率。输出长度为 n//2+1。', ret:'一维COMPLEX128数组，长度=n/2+1（n为输入长度，必须为偶数）', params:['实数输入（任意实数dtype）']},
  irfft:    { desc:'逆实数 FFT。从半频谱重建长度为 n 的实数信号。' , ret:'一维 FLOAT64 数组，长度为 n', params:['半频谱（长度n/2+1）','Desired output length (must be even)']},
  fftfreq:  { desc:'FFT 采样频率。freq[i]=i/(n*d) if i<n/2 else (i-n)/(n*d)。' , ret:'一维 FLOAT64 数组，长度为 n', params:['窗口长度','Sample spacing (default 1.0)']},
  fftshift: { desc:'将零频分量移到频谱中心。每个轴移动 n/2。' , notes:'每个轴平移 n//2。对 fftfreq 输出使用平移可得居中的频率。', ret:'与输入相同形状的数组', params:['频域数组']},
  fft2:     { desc:'二维前向 FFT。在最后两个轴上计算。类似 numpy.fft.fft2。' , ret:'COMPLEX128 数组，与输入形状相同', params:['输入（转为COMPLEX128）']},
  fftn:     { desc:'N 维前向 FFT。在所有轴上计算。类似 numpy.fft.fftn。' , ret:'COMPLEX128 数组，与输入形状相同', params:['输入（转为COMPLEX128）']},
  hfft:     { desc:'Hermitian FFT。从半频谱重建实数信号。类似 numpy.fft.hfft。' , params:['半Hermitian频谱','Output length']},
  ifft2:    { desc:'二维逆 FFT。在最后两个轴上计算。类似 numpy.fft.ifft2。' , params:['COMPLEX128频谱']},
  ifftn:    { desc:'N 维逆 FFT。在所有轴上计算。类似 numpy.fft.ifftn。' , params:['COMPLEX128频谱']},
  ifftshift:{ desc:'fftshift 的逆操作。将零频从中心移回原点。类似 numpy.fft.ifftshift。' , params:['要逆平移的数组']},
  ihfft:    { desc:'逆 Hermitian FFT。实数信号→归一化半频谱。类似 numpy.fft.ihfft。' , params:['实数输入','Output length']},
  irfft2:   { desc:'二维实数逆 FFT。从半频谱数据还原实数二维信号。类似 numpy.fft.irfft2。' , params:['半频谱','Output shape {rows,cols}']},
  irfftn:   { desc:'N 维实数逆 FFT。从半频谱数据还原实数 N 维信号。类似 numpy.fft.irfftn。' , params:['半频谱','Output shape array']},
  rfft2:    { desc:'二维实数 FFT。输入为实数，仅返回非负频率。类似 numpy.fft.rfft2。' , params:['实数输入（二维）']},
  rfftfreq: { desc:'实数 FFT 的采样频率。仅返回非负频率。类似 numpy.fft.rfftfreq。' , ret:'一维FLOAT64数组，长度=n/2+1', params:['窗口长度','Sample spacing']},
  rfftn:    { desc:'N 维实数 FFT。输入为实数，仅返回非负频率。类似 numpy.fft.rfftn。' , params:['实数输入（N维）']},

  // Datetime
  datetime_now: { desc:'返回当前系统时间（DateTime64 值）。值以纳秒存储，单位 DT_UNIT_NS。' , notes:'通过 time() 读取系统时钟。精度始终为纳秒。', ret:'DateTime64结构体，纳秒值，单位=DT_UNIT_NS'},
  datetime_from_string: { desc:'将 ISO 8601 日期时间字符串解析为 DateTime64 值。支持仅日期、日期时间和带小数秒的格式。' , notes:'支持纯日期、日期时间和带小数秒的格式。不支持时区。', ret:'DateTime64结构体，解析失败时value=0', params:['ISO 8601格式字符串，如"2025-04-27"','Storage resolution — DT_UNIT_NS, DT_UNIT_S, DT_UNIT_D, etc.']},
  datetime_as_string: { desc:'将 DateTime64 格式化为 ISO 8601 字符串。调用者需 free() 返回的字符串。' , notes:'格式取决于单位：日期→"YYYY-MM-DD"，秒→"YYYY-MM-DDTHH:MM:SS"，纳秒→含小数秒。', ret:'malloc', params:['指向要格式化的DateTime64指针']},
  datetime_add_days: { desc:'对 DateTime64 值加减指定天数。正确处理月/年边界。' , notes:'通过归一化到天数正确处理跨月/跨年边界。', ret:'按天数偏移后的新DateTime64值', params:['基准日期时间','Days to add (positive) or subtract (negative)']},
  datetime_sub_days: { desc:'从 DateTime64 值减去天数。等价于 add_days(dt, -days)。' , ret:'新的DateTime64值', params:['基准日期时间','Days to subtract']},
  datetime_eq: { desc:'比较两个 DateTime64 是否表示同一时刻。以纳秒精度比较。' , notes:'比较以纳秒为单位，不受存储单位影响。', ret:'布尔值——a和b代表相同时刻为true', params:['第一个日期时间','Second datetime']},
  datetime_lt: { desc:'判断 a 是否严格早于 b。' , ret:'布尔值', params:['第一个','Second']},
  datetime_gt: { desc:'判断 a 是否严格晚于 b。' , ret:'布尔值', params:['第一个','Second']},
  datetime_to_ns: { desc:'将 DateTime64 转换为自 1970-01-01 以来的纳秒数。' , notes:'用于以原始纳秒形式进行算术运算和比较。', ret:'int64——自1970-01-01以来的纳秒总数', params:['日期时间值']},
  is_busday: { desc:'检查日期数组中的每一天是否为工作日。类似 numpy.is_busday。支持自定义 weekmask 和节假日。' , notes:'weekmask 字符：周一至周日。默认 "1111100"=周六日休息。holidays 数组可选。', ret:'一维BOOL数组，与dates长度相同', params:['一维DATETIME64数组','7-char string, 1=workday 0=holiday, e.g. "1111100" for Mon-Fri','1-D DATETIME64 array of holidays (NULL=none)']},

  // Random
  random_seed:{ desc:'使用 splitmix64 初始化全局 xoshiro256** 随机数发生器。' , notes:'状态非线程安全。可复现性：相同种子→相同序列。', params:['64位种子值']},
  random_sample:{ desc:'[0,1) 均匀分布。类似 numpy.random.random_sample。' , ret:'FLOAT64 类型的结果数组', params:['输出形状','维度数']},
  random_randn:{ desc:'标准正态 N(0,1)。Box-Muller 变换。' , ret:'FLOAT64 类型的结果数组', params:['输出形状','Dimensions']},
  random_normal:{ desc:'正态分布 N(loc, scale²)。Box-Muller 变换。' , ret:'FLOAT64 类型的结果数组', params:['均值（μ）','Standard deviation (σ, must be > 0)','输出形状','Dimensions']},
  random_uniform:{ desc:'[low, high) 均匀分布。' , ret:'FLOAT64 类型的结果数组', params:['下界','Upper bound','输出形状','Dimensions']},
  random_gamma:{ desc:'Gamma 分布。shape≥1 用 Marsaglia-Tsang，shape<1 用 Johnk。' , notes:'k≥1 使用 Marsaglia-Tsang(2000) 快速接受-拒绝法。k<1 使用 Johnk 生成器。', ret:'FLOAT64 类型的结果数组', params:['形状参数（k>0）','Scale parameter (θ, >0); mean = k*θ','输出形状','Dimensions']},
  random_beta:{ desc:'Beta 分布。X = G1/(G1+G2)，G1~Gamma(a)，G2~Gamma(b)。' , ret:'FLOAT64数组，范围为[0,1]', params:['形状参数1（α>0）','Shape 2 (β, >0)','输出形状','Dimensions']},
  random_binomial:{ desc:'二项分布 Binomial(n,p)。np(1-p)>30 时用正态近似。' , notes:'n*p*(1-p)>30 时使用正态近似以提高效率。', ret:'INT64 类型的结果数组', params:['试验次数','Success probability (0..1)','输出形状','Dimensions']},
  random_poisson:{ desc:'泊松分布。λ<30 用 Knuth 算法，大 λ 用正态近似。' , ret:'INT64 类型的结果数组', params:['率（λ>0）——均值=方差=λ','输出形状','Dimensions']},
  random_permutation:{ desc:'随机排列（返回新副本）。Fisher-Yates 洗牌算法。' , notes:'O(n) Fisher-Yates(Knuth) 洗牌。不修改 x——返回新数组。', ret:'打乱顺序后的新数组，与原数组相同形状和dtype', params:['输入（任意dtype）']},
  random_shuffle:{ desc:'原地 Fisher-Yates 洗牌。' , notes:'O(n)。原地修改 x——不制作副本。', params:['要打乱的数组（原地修改）']},
  random_choice:{ desc:'随机抽样。支持有放回/无放回和加权抽样。' , notes:'p 必须总和为 1（如提供）且长度与展平后的 a 一致。替换为1时有放回抽样。', ret:'FLOAT64数组，长度为size', params:['总体（任意形状，展平）','Number of samples to draw','1=with replacement (bootstrap), 0=without','Probability weights (NULL=uniform)']},
  random_multivariate_normal:{ desc:'多元正态分布（通过 Cholesky 分解）。μ + L·z, z~N(0,I)。' , notes:'通过 cholesky(cov) 得到 L，再变换标准正态。协方差非正定→返回 NULL。', ret:'数组，最后一维长度=K（均值长度）', params:['一维均值向量（μ，长度为K）','K×K covariance matrix (Σ, must be SPD)','Batch shape','Number of batch dims']},
  random_dirichlet:{ desc:'Dirichlet 分布（通过 Gamma 归一化）。每行和为 1。' , ret:'数组，最后一维长度=len(alpha)，每行和为1', params:['一维集中度参数（αⱼ>0）','Batch shape','Number of batch dims']},
  random_bytes:{ desc:'返回指定长度的随机字节数组。' , notes:'每个字节从 random_uint64() 输出中提取。', ret:'n字节数组（dtype=UINT8）', params:['要生成的字节数']},
  random_chisquare:{ desc:'从卡方分布 χ²(df) 中抽取随机样本。' , ret:'FLOAT64 类型的结果数组', params:['自由度（k>0）','输出形状','Dimensions']},
  random_double:{ desc:'返回一个 [0, 1) 内的均匀随机双精度浮点数。' , ret:'[0.0, 1.0)内的双精度浮点数'},
  random_exponential:{ desc:'从指数分布 Exp(scale) 中抽取随机样本。' , ret:'FLOAT64 类型的结果数组', params:['尺度参数（β>0）；亦=1/λ','输出形状','Dimensions']},
  random_f:{ desc:'从 F 分布 F(dfn, dfd) 中抽取随机样本。' , ret:'FLOAT64 类型的结果数组', params:['分子自由度','Denominator df','输出形状','Dimensions']},
  random_geometric:{ desc:'从几何分布 Geometric(p) 中抽取随机样本。返回首次成功前的失败次数。' , ret:'INT64 类型的结果数组', params:['成功概率（0<p<=1）','输出形状','Dimensions']},
  random_gumbel:{ desc:'从 Gumbel 分布中抽取随机样本。常用于极值建模。' , ret:'FLOAT64 类型的结果数组', params:['位置（μ）','Scale (σ)','输出形状','Dimensions']},
  random_hypergeometric:{ desc:'从超几何分布中抽取随机样本。不放回抽样。' , ret:'INT64 类型的结果数组', params:['总体中好物品的数量','Number of bad items in population','Number of draws without replacement','输出形状','Dimensions']},
  random_laplace:{ desc:'从 Laplace（双指数）分布中抽取随机样本。' , ret:'FLOAT64 类型的结果数组', params:['位置（μ）','Scale (σ, >0)','输出形状','Dimensions']},
  random_logistic:{ desc:'从 Logistic 分布中抽取随机样本。' , ret:'FLOAT64 类型的结果数组', params:['位置','Scale (>0)','输出形状','Dimensions']},
  random_lognormal:{ desc:'从对数正态分布中抽取随机样本。log(X) 服从 N(mean, sigma²)。' , ret:'FLOAT64 类型的结果数组', params:['底正态分布的均值','Std of underlying normal (>0)','输出形状','Dimensions']},
  random_logseries:{ desc:'从对数级数分布中抽取随机样本。' , ret:'INT64 类型的结果数组', params:['参数（0<p<1）','输出形状','Dimensions']},
  random_multinomial:{ desc:'从多项分布中抽取随机样本。每次试验的计数。' , ret:'数组，形状=shape+[K]，每行和为n', params:['试验次数','1-D probability vector (length K, must sum to 1)','Batch shape (prefix dimensions)','Number of batch dimensions']},
  random_negative_binomial:{ desc:'从负二项分布中抽取随机样本。第 n 次成功前的失败次数。' , ret:'INT64 类型的结果数组', params:['目标成功次数','Success probability','输出形状','Dimensions']},
  random_pareto:{ desc:'从 Pareto（幂律）分布中抽取随机样本。形状参数 a 控制尾部厚度。' , ret:'FLOAT64 类型的结果数组', params:['形状参数（>0）','输出形状','Dimensions']},
  random_power:{ desc:'从幂分布中抽取随机样本，a>0 时形状由 a 控制。' , ret:'FLOAT64数组，范围为[0,1]', params:['形状参数（>0）','输出形状','Dimensions']},
  random_rand:{ desc:'random_sample 的别名。返回 [0,1) 均匀随机数。' , params:['输出形状','Dims']},
  random_randint:{ desc:'从 [low, high) 中抽取均匀整数随机样本。指定 dtype 控制输出类型。' , ret:'指定类型的整数数组', params:['下界（包含）','Upper bound (exclusive)','输出形状','Dimensions','INT32, INT64, etc.']},
  random_rayleigh:{ desc:'从 Rayleigh 分布中抽取随机样本。常用于信号衰落建模。' , ret:'FLOAT64 类型的结果数组', params:['尺度（σ>0）；mode=σ','输出形状','Dimensions']},
  random_standard_cauchy:{ desc:'从标准 Cauchy 分布中抽取随机样本。尾部极重，无有限均值。' , notes:'无均值、无方差——分布具有病态性质。请谨慎使用。', ret:'FLOAT64 类型的结果数组', params:['输出形状','Dimensions']},
  random_standard_exponential:{ desc:'从标准指数分布 Exp(1) 中抽取随机样本。' , params:['输出形状','Dims']},
  random_standard_gamma:{ desc:'从标准 Gamma(shape, scale=1) 分布中抽取随机样本。' , params:['形状参数（k>0）','输出形状','Dims']},
  random_standard_normal:{ desc:'评估标准正态分布 N(0,1) 的样本。每样本使用一个随机数。' , params:['输出形状','Dims']},
  random_standard_t:{ desc:'从学生 t 分布 t(df) 中抽取随机样本。' , ret:'FLOAT64 类型的结果数组', params:['自由度（>0）','输出形状','Dimensions']},
  random_triangular:{ desc:'从三角分布中抽取随机样本，在 [left, right] 内以 mode 为众数。' , ret:'FLOAT64 类型的结果数组', params:['下界','Peak location (left ≤ mode ≤ right)','Upper bound','输出形状','Dimensions']},
  random_uint64:{ desc:'返回一个均匀随机的 64 位无符号整数。' , ret:'[0, 2^64-1]内的无符号64位整数'},
  random_vonmises:{ desc:'从 von Mises（环形正态）分布中抽取随机样本。用于方向数据。' , ret:'FLOAT64数组，范围为[-pi, pi]', params:['均值方向（弧度，-pi到pi）','Concentration (≥0; 0=uniform on circle)','输出形状','Dimensions']},
  random_wald:{ desc:'从 Wald（逆高斯）分布中抽取随机样本。用于首次通过时间建模。' , ret:'FLOAT64 类型的结果数组', params:['均值（μ>0）','Scale (λ, >0)','输出形状','Dimensions']},
  random_weibull:{ desc:'从 Weibull 分布中抽取随机样本。形状参数 a 控制失效率变化。' , ret:'FLOAT64 类型的结果数组', params:['形状参数（k>0）；k=1=指数分布, k=2=Rayleigh分布','输出形状','Dimensions']},
  random_zipf:{ desc:'从 Zipf 分布中抽取随机样本。a 控制偏斜程度。' , ret:'INT64 类型的结果数组', params:['指数（>1）','输出形状','Dimensions']},

  // Polynomial
  poly:     { desc:'从根求多项式系数。(x-r₁)(x-r₂)⋯(x-rₙ)=x^n+coeff[1]x^(n-1)+⋯。' },
  roots:    { desc:'通过伴随矩阵 + Francis 双位移 QR 算法求多项式根。', notes:'构建 Hessenberg 伴随矩阵，内置隐式 QR。支持实根、复根和重根。前导零会被去除。' , ret:'一维COMPLEX128数组，长度=度数', params:['1-D coefficients in descending powers (p[0]*x^n + … + p[n])']},
  polyfit:  { desc:'最小二乘多项式拟合。最小化 ||V·c - y||²。' , notes:'使用法方程：V^T·V·c=V^T·y。高次或病态拟合建议考虑其他方法。', ret:'一维FLOAT64数组，长度=deg+1（降幂排列）', params:['x坐标（与y长度相同）','y坐标','Polynomial degree (must be < len(x))']},
  polyval:  { desc:'用 Horner 方法计算多项式在点 x 处的值。' , notes:'Horner 法：p(x)=((p[0]*x+p[1])*x+…)+p[n]。复杂度 O(deg×len(x))。', ret:'FLOAT64 数组，与 x 形状相同', params:['系数（降幂排列）','Evaluation points (any shape)']},

  // Windows
  hamming:  { desc:'Hamming 窗：w[n]=0.54−0.46cos(2πn/(M−1))。旁瓣 −42.7 dB。' , notes:'旁瓣电平：−42.7dB。常用于语音处理。端点不为零。', ret:'一维 FLOAT64 数组，长度为 m', params:['窗口长度（>=1）']},
  hanning:  { desc:'Hann 窗：w[n]=0.5−0.5cos(2πn/(M−1))。旁瓣 −31.5 dB。' , notes:'也称 "Hann 窗"（以 Julius von Hann 命名）。旁瓣：−31.5dB。端点为零。', ret:'一维 FLOAT64 数组，长度为 m', params:['窗口长度（>=1）']},

  // Interpolation & Signal
  interp:   { desc:'一维线性插值。xp 必须单调递增。', notes:'使用 find_interval() 二分查找，O(log n) 每点。xp 必须有序——不进行验证。' },
  convolve: { desc:'一维线性卷积（mode="full"）。result[k]=Σa[i]·v[k-i]。', notes:'两输入先展平并转为 FLOAT64。O(n1·n2) 朴素算法。' , ret:'一维FLOAT64数组，长度=len(a)+len(v)-1', params:['第一个数组（展平，任意dtype=转为FLOAT64）','Second array (flattened, any dtype → FLOAT64)']},

  // Comparison & Search
  greater:  { desc:'逐元素 >（返回 BOOL 数组）。' , ret:'BOOL 类型广播形状的结果数组', params:['第一个','Second']},
  equal:    { desc:'逐元素 ==（返回 BOOL 数组）。' },
  all:      { desc:'测试沿轴所有元素是否为真（≠0）。' , ret:'BOOL 类型数组', params:['输入','Axis (-1=flat)','Keep dims']},
  any:      { desc:'测试沿轴是否有元素为真。' , ret:'BOOL 类型数组', params:['输入','Axis (-1=flat)','Keep reduced dims']},
  where:    { desc:'根据条件从 x 或 y 中选择元素。三参数形式。' , notes:'三个输入均广播到公共形状。这是三参数形式。', ret:'广播形状的数组，dtype自动提升', params:['BOOL条件数组','Values where true (broadcastable)','Values where false (broadcastable)']},
  unique:   { desc:'找到排序去重后的唯一元素。返回包含排序去重元素的 UniqueResult 结构体。', ret:'UniqueResult 结构体，包含 values、indices、inverse、counts', params:['输入（展平）','If 1, also return indices of first occurrences','If 1, return indices to reconstruct input','If 1, return count of each unique value']},
  intersect1d:{ desc:'两个数组的排序唯一交集。实数→FLOAT64，复数→COMPLEX128。', notes:'双指针归并 O(n1+n2)。空输入→空输出。复数比较按实部优先字典序。' , ret:'一维数组：实数输入为FLOAT64，任一输入为复数时为COMPLEX128', params:['第一个输入（任意形状/dtype）','Second input','If non-zero, skip sort+dedup (inputs must already be sorted and unique)']},
  searchsorted:{ desc:'二分查找插入位置。side="left" 返回第一个≥的位置，"right" 返回第一个>的位置。' , notes:'a 必须是有序的——不做验证。二分查找 O(n·log m)。', ret:'INT64数组，与v同形状，索引范围[0, len(a)]', params:['一维有序数组（实数）','Values to locate (any shape)','"left"=first suitable index; "right"=last suitable index']},
  digitize: { desc:'返回每个值所在分箱的索引。right=0 左闭右开 [)，right=1 左开右闭 (]。' , ret:'INT64数组，与arr同形状，索引范围[0, len(bins)]', params:['要分箱的值（任意形状，任意实数dtype）','1-D monotonically increasing bin edges','0=left-inclusive intervals [...]), 1=right-inclusive (...]']},
  argmax:   { desc:'沿轴最大值索引。' , ret:'INT64索引数组', params:['输入','Axis (-1=flat)','Keep dims']},

  // Manipulation
  clip:     { desc:'将数组值裁剪到 [min, max] 区间。等价于 maximum(min, minimum(arr, max))。' , notes:'等价于 maximum(min, minimum(arr, max))。不检查 min≤max。', ret:'与arr相同形状和dtype', params:['输入','Lower bound (broadcastable)','Upper bound (broadcastable)']},
  extract:  { desc:'提取条件为真的元素（展平一维输出）。' , notes:'两个数组都先展平。结果始终为一维。', ret:'一维数组，相同dtype，长度=#true', params:['BOOL条件','Source values']},
  append:   { desc:'沿指定轴在数组末尾追加值。类似 numpy.append。' , params:['底数','Values to append','Axis (-1=flatten both first)']},
  around:   { desc:'将数组元素舍入到指定小数位数。偶数优先（银行家舍入）。类似 numpy.around。' , params:['输入','Decimal places (0=nearest integer)']},
  choose:   { desc:'根据索引数组从一组选项中选取元素。类似 numpy.choose。' , ret:'广播形状的数组，dtype自动提升', params:['INT64索引数组（0..num_choices-1）','Number of choices','Array of arrays (must be broadcastable)','0=RAISE, 1=WRAP, 2=CLIP for out-of-range indices']},
  compress: { desc:'沿指定轴返回数组中满足条件的切片。类似 numpy.compress。' , ret:'沿轴缩减到True值数量的数组', params:['一维BOOL数组（必要时先展平）','输入','Axis (0 for 1-D)']},
  npc_delete:{ desc:'沿指定轴删除子数组并返回新数组。类似 numpy.delete。为 C++ 兼容而从 delete 重命名。' , ret:'具有 obj entries removed along axis 的新数组', params:['输入','INT64 indices to delete','Axis to delete along']},
  fix:      { desc:'向零舍入到最接近的整数。类似 numpy.fix。' , params:['输入']},
  insert:   { desc:'沿指定轴在给定索引前插入值。类似 numpy.insert。' , ret:'具有 values inserted 的新数组', params:['输入','INT64 indices (where to insert before)','Values to insert (must match along other axes)','Axis']},
  place:    { desc:'根据掩码条件将数组中对应位置替换为指定值。类似 numpy.place。' , ret:'成功返回0', params:['要修改的数组（原地）','BOOL mask','Values to place (repeated cyclically if shorter)']},
  put:      { desc:'沿展平数组在指定索引处替换值。类似 numpy.put。支持 raise/clip/wrap 模式。' , ret:'成功返回0', params:['数组（原地修改）','INT64 flat indices','Replacement values','"raise" "wrap" "clip"']},
  putmask:  { desc:'根据布尔掩码替换数组中对应元素。类似 numpy.putmask。' , params:['要修改的数组','BOOL mask','Source values (broadcast to arr shape)']},
  trim_zeros:{ desc:'去除数组首尾的零。可选 f（前）、b（后）、fb（两端）。类似 numpy.trim_zeros。' , ret:'一维数组（arr的子集）', params:['一维输入','"f" (front only), "b" (back only), "fb" (both)']},

  // Utilities
  nan_to_num:{ desc:'替换 NaN→0，+Inf→MAX，-Inf→-MAX。支持复数。' , notes:'复数：实部或虚部为 NaN→该分量置零。Inf→对应类型的最大值(保留符号)。', ret:'与输入相同的形状和数据类型', params:['输入（任意dtype，含复数）']},
  unwrap:   { desc:'相位解缠绕。将绝对值 >discont 的跳变修正为 2π 的整数倍。' , notes:'周期始终为 2π。discont 是检测阈值而非周期。跳变>discont→加/减 2π 实现解缠绕。', ret:'与输入相同的形状和数据类型', params:['相位角','Maximum discontinuity (default=π)','Axis along which to unwrap']},

  // Statistics (additional)
  average:  { desc:'计算沿指定轴的加权平均值。类似 numpy.average。' , ret:'FLOAT64 类型的结果数组', params:['输入','Weights (must match along axis; NULL = uniform)','Axis (-1=flat)','Keep dims']},
  diff:     { desc:'沿指定轴计算第 n 阶离散差分。类似 numpy.diff。' , ret:'沿轴缩减n个元素的数组', params:['输入','Order (1=first difference, 2=second, etc.)','Axis']},
  max:      { desc:'沿指定轴返回最大值。axis=-1 返回全局最大值。' , params:['输入','Axis (-1=flat)','Keep dims']},
  maximum:  { desc:'逐元素取最大值，支持广播。类似 numpy.maximum。' , ret:'与输入相同dtype（提升后），广播形状', params:['第一个','Second']},
  min:      { desc:'沿指定轴返回最小值。axis=-1 返回全局最小值。' , ret:'在指定轴上归约/连接的结果数组', params:['输入','Axis (-1=flat)','Keep dims']},
  minimum:  { desc:'逐元素取最小值，支持广播。类似 numpy.minimum。' , params:['第一个','Second']},
  prod:     { desc:'沿指定轴计算乘积. axis=-1 返回所有元素的乘积。' , ret:'在指定轴上归约/连接的结果数组', params:['输入','Axis (-1=all)','Keep reduced dims']},
  ptp:      { desc:'沿指定轴计算峰峰值（最大值-最小值）。类似 numpy.ptp。' , params:['输入','Axis','Keep dims']},
  trapz:    { desc:'使用梯形法则沿指定轴进行数值积分。类似 numpy.trapz。' , ret:'FLOAT64数组，形状=y.shape去掉轴维度', params:['被积函数值','Sample positions (NULL = use uniform dx)','Uniform sample spacing (used when x is NULL)','Axis to integrate along']},

  // NaN Statistics
  nancumprod:{ desc:'沿指定轴计算累积乘积，忽略 NaN。类似 numpy.nancumprod。' , params:['输入','Axis (-1=flat)']},
  nancumsum:{ desc:'沿指定轴计算累积和，忽略 NaN。类似 numpy.nancumsum。' , ret:'与输入相同的形状', params:['输入','Axis (-1=flat)']},
  nanmax:   { desc:'沿指定轴求最大值，忽略 NaN。类似 numpy.nanmax。' , ret:'FLOAT64数组。全NaN切片返回NaN', params:['输入（实数或复数）','Axis (-1=flat)']},
  nanmean:  { desc:'沿指定轴计算算术平均值，忽略 NaN。类似 numpy.nanmean。' , params:['输入','Axis (-1=flat)']},
  nanmedian:{ desc:'沿指定轴计算中位数，忽略 NaN。类似 numpy.nanmedian。' , params:['输入','Axis (-1=flat)']},
  nanmin:   { desc:'沿指定轴求最小值，忽略 NaN。类似 numpy.nanmin。' , params:['输入','Axis (-1=flat)']},
  nanpercentile:{ desc:'计算第 q 百分位数，忽略 NaN。q∈[0,100]。类似 numpy.nanpercentile。' , ret:'FLOAT64 类型的结果数组', params:['输入','1D FLOAT64, 0–100','Axis']},
  nanquantile:{ desc:'计算第 q 分位数，忽略 NaN。q∈[0,1]。类似 numpy.nanquantile。' , params:['输入','1D FLOAT64 (0-1)','Axis']},

  // Linear Algebra (additional)
  conj_transpose:{ desc:'共轭转置。实数等价于 transpose，复数同时取共轭。' , ret:'二维数组，轴交换且复数取共轭', params:['输入（二维）']},
  diag_indices:{ desc:'返回 n 维对角线索引元组。用于访问多维数组的对角线元素。' , params:['大小','Number of dims','Output: array count']},
  diagonal: { desc:'沿指定轴偏移返回对角线元素。类似 numpy.diagonal。' , ret:'一维对角线元素数组', params:['输入（>=二维）','Diagonal offset','First axis','Second axis']},
  eig:      { desc:'计算方阵的特征值和特征向量。使用 Francis QR 算法。类似 numpy.linalg.eig。' , notes:'对称矩阵使用纯 C Jacobi 法；一般矩阵需 -DUSE_LAPACK 链接 -llapack -lblas。', ret:'特征值（COMPLEX128）和特征向量', params:['nxn矩阵（实数，FLOAT32/FLOAT64）']},
  inner:    { desc:'两个数组的内积。对最后轴求和。类似 numpy.inner。' , params:['第一个','Second']},
  tensordot:{ desc:'沿指定轴对两个数组进行张量点积。类似 numpy.tensordot。' , ret:'收缩张量', params:['第一个张量','Second tensor','1D INT64 axes of a to contract','1D INT64 axes of b to contract (same length as axes_a)']},
  tril:     { desc:'返回数组的下三角部分，其余置零。类似 numpy.tril。' , ret:'与输入相同形状和dtype的数组', params:['输入','Diagonal offset (0=main, >0=more, <0=less)']},
  tril_indices:{ desc:'返回下三角索引，用于 n×m 矩阵。类似 numpy.tril_indices。' , ret:'2xN INT64数组（[0]为行索引，[1]为列索引）', params:['行','Diagonal offset','Columns']},
  tril_indices_from:{ desc:'从给定数组获取下三角索引。类似 numpy.tril_indices_from。' , params:['现有数组','Offset']},
  triu:     { desc:'返回数组的上三角部分，其余置零。类似 numpy.triu。' , params:['输入','Diagonal offset']},
  triu_indices:{ desc:'返回上三角索引，用于 n×m 矩阵。类似 numpy.triu_indices。' , params:['行','Offset','Cols']},
  triu_indices_from:{ desc:'从给定数组获取上三角索引。类似 numpy.triu_indices_from。' , params:['现有数组','Offset']},
  vdot:     { desc:'共轭内积。展平输入后计算点积，对第一个参数取共轭。类似 numpy.vdot。' , notes:'实输入：展平后与 dot 相同。复输入：第一个参数取共轭。', ret:'标量（ndim=0）FLOAT64或COMPLEX128数组', params:['第一个（展平）','Second (flattened)']},

  // Comparison (additional)
  count_nonzero:{ desc:'沿指定轴统计非零元素个数。类似 numpy.count_nonzero。' , ret:'INT64 类型的结果数组', params:['输入','Axis (-1=flat)']},
  greater_equal:{ desc:'逐元素比较 x1 >= x2，返回布尔数组。' , params:['第一个','Second']},
  isclose:  { desc:'逐元素检查两数组是否在容差内相等。支持相对/绝对容差及 NaN 处理。类似 numpy.isclose。' , ret:'BOOL 类型广播形状的结果数组', params:['第一个','Second','Relative tolerance (e.g. 1e-5)','Absolute tolerance (e.g. 1e-8)','If 1, NaN==NaN returns true']},
  less:     { desc:'逐元素比较 x1 < x2，返回布尔数组。类似 numpy.less。' , params:['第一个','Second']},
  less_equal:{ desc:'逐元素比较 x1 <= x2，返回布尔数组。' , params:['第一个','Second']},
  logical_and:{ desc:'逐元素逻辑与。非零视为 True。类似 numpy.logical_and。' , ret:'BOOL 类型数组', params:['第一个','Second']},
  logical_not:{ desc:'逐元素逻辑非。非零→0，零→1。类似 numpy.logical_not。' , params:['输入']},
  logical_or:{ desc:'逐元素逻辑或。任一非零即返回 1。类似 numpy.logical_or。' , params:['第一个','Second']},
  logical_xor:{ desc:'逐元素逻辑异或。类似 numpy.logical_xor。' , params:['第一个','Second']},

  // Floating Point (additional)
  copysign_array:{ desc:'将 x1 的符号改为 x2 的符号，值不变。类似 numpy.copysign。' , ret:'FLOAT64 类型广播形状的结果数组', params:['幅度来源','Sign source']},
  fabs_array:{ desc:'逐元素绝对值（实数）。复数请用 abs_array。类似 numpy.fabs。' , ret:'与输入相同的数据类型', params:['浮点输入']},
  frexp_array:{ desc:'将浮点数分解为尾数和指数：x = mant * 2^exp。类似 numpy.frexp。' , ret:'成功返回0', params:['输入','Output mantissa (FLOAT64)','Output exponent (INT32)']},
  modf_array:{ desc:'将浮点数分解为小数部分和整数部分，均保持原符号。类似 numpy.modf。' , ret:'成功返回0', params:['输入','Fractional part (FLOAT64)','Integral part (FLOAT64)']},
  nextafter_array:{ desc:'返回 x1 朝 x2 方向的下一个可表示的浮点值。类似 numpy.nextafter。' , ret:'FLOAT64 类型广播形状的结果数组', params:['起始值','Direction']},
  spacing_array:{ desc:'返回 x 到下一个相邻浮点数的距离。类似 numpy.spacing。' , params:['浮点输入']},
  npc_frexp:{ desc:'将浮点数分解为尾数和指数。类似 numpy.frexp。' },
  spacing:{ desc:'返回每个值到下一个相邻浮点数的距离。类似 numpy.spacing。' },

  // Bitwise
  bitwise_and:{ desc:'逐元素按位与。类似 numpy.bitwise_and。' , ret:'与输入相同dtype，广播形状', params:['第一个（整数）','Second (integer)']},
  bitwise_not:{ desc:'逐元素按位取反。类似 numpy.bitwise_not。' , ret:'与输入相同的形状和数据类型', params:['整数数组']},
  bitwise_or:{ desc:'逐元素按位或。类似 numpy.bitwise_or。' , params:['第一个（整数）','Second (integer)']},
  bitwise_xor:{ desc:'逐元素按位异或。类似 numpy.bitwise_xor。' , params:['第一个（整数）','Second (integer)']},
  invert:   { desc:'逐元素按位取反（bitwise_not 的别名）。类似 numpy.invert。' , params:['整数数组']},
  left_shift:{ desc:'逐元素左移位。x1 << x2。类似 numpy.left_shift。' , params:['底数（整数）','Shift (integer)']},
  right_shift:{ desc:'逐元素右移位（算术移位）。x1 >> x2。类似 numpy.right_shift。' , params:['底数（整数）','Shift (integer)']},

  // Utilities (additional)
  aldexp:   { desc:'计算 x1 * 2^x2。用于以 2 为底的缩放。' , ret:'INT64 类型广播形状的结果数组', params:['整数底','Integer shift amount']},
  bessel_i0:{ desc:'计算修正贝塞尔函数 I0(x)（标量版）。' , ret:'I0(x) 值（双精度浮点）', params:['实数值']},
  bincount: { desc:'统计非负整数数组中每个值出现的次数。类似 numpy.bincount。' , ret:'一维 FLOAT64 数组', params:['一维非负整数','Optional — weight for each occurrence (NULL=count 1)','Minimum output length (padded with zeros)']},
  check_finite:{ desc:'检查数组中元素的有限性。分别实现为 isnan_array、isinf_array、isfinite_array。' , params:['输入','Output: 1=all finite, 0=has NaN/Inf']},
  ediff1d:  { desc:'计算一维数组相邻元素的差值。可选项前后拼接。类似 numpy.ediff1d。' , ret:'一维 FLOAT64 数组', params:['输入（展平）','Prepend before diff result (NULL=none)','Append after diff result (NULL=none)']},
  fill_diagonal:{ desc:'原地填充任意二维数组的对角线。支持非方阵。类似 numpy.fill_diagonal。' , params:['二维数组（原地修改）','Pointer to fill value (type must match arr->dtype)']},
  gcd_array:{ desc:'逐元素计算最大公约数。类似 numpy.gcd。' , ret:'INT64 类型广播形状的结果数组', params:['第一个整数','Second integer']},
  i0:       { desc:'逐元素计算第一类零阶修正贝塞尔函数 I0(x)。' , notes:'使用多项式近似，精度约 2e-16。kaiser 窗内部调用此函数。', ret:'FLOAT64 类型的结果数组', params:['实数输入']},
  imag_array:{ desc:'返回复数数组的虚部。类似 numpy.imag。' , params:['输入（任意dtype）']},
  ix_:      { desc:'构造开网格索引，用于多维索引。类似 numpy.ix_。' , ret:'成功返回0', params:['n个一维索引数组','Number of arrays','Output: reshaped arrays with newaxes for broadcasting']},
  lcm:      { desc:'逐元素计算最小公倍数。类似 numpy.lcm。' , notes:'lcm(a,b)=|a·b|/gcd(a,b)。大值时可能溢出（与 NumPy 行为一致）。', ret:'INT64 类型广播形状的结果数组', params:['第一个整数','Second integer']},
  piecewise:{ desc:'分段函数求值。根据条件列表选择对应函数。类似 numpy.piecewise。' , notes:'条件按顺序求值，首个为真的条件胜出。所有条件为假则返回默认值。', ret:'FLOAT64 数组，与 x 形状相同', params:['输入','ncond BOOL condition arrays','Number of conditions','ncond function pointers: double (*)(double)']},
  real_array:{ desc:'返回复数数组的实部。类似 numpy.real。' , params:['输入']},
  ndim:      { desc:'返回数组的维度数。类似 numpy.ndim。' },

  // Search (additional)
  argmin:   { desc:'沿指定轴返回最小值的索引。类似 numpy.argmin。' , params:['输入','Axis (-1=flat)','Keep dims']},
  argpartition:{ desc:'沿指定轴进行部分排序并返回索引。类似 numpy.argpartition。' , notes:'只需 k 个最小/最大元素时比 argsort 更快。', ret:'INT64 索引数组，与输入形状相同', params:['输入','Pivot position','Axis']},
  argsort:  { desc:'沿指定轴返回排序后的索引。支持 quicksort 和 mergesort。类似 numpy.argsort。' , notes:'Mergesort 是稳定的：等值键保持原始相对顺序。', ret:'INT64 索引数组，与输入形状相同', params:['输入','Axis to sort','"quicksort" (default) or "mergesort" (stable)']},
  argwhere: { desc:'返回非零元素的索引（分组排列）。类似 numpy.argwhere。' , ret:'二维INT64数组，形状为(n_nonzero, ndim)', params:['输入（非零元素为True）']},
  setdiff1d:{ desc:'计算两个数组的差集。返回 arr1 中有而 arr2 中没有的元素。类似 numpy.setdiff1d。' , ret:'一维 FLOAT64 数组', params:['第一个集合','Set to exclude']},

  // Polynomial (additional)
  polyadd:  { desc:'多项式加法。对齐系数后相加。类似 numpy.polyadd。' , ret:'一维FLOAT64数组，长度=max(len(a), len(b))', params:['第一个多项式系数','Second polynomial coeffs']},
  polyder:  { desc:'多项式求导。m 指定求导阶数。类似 numpy.polyder。' , ret:'一维FLOAT64数组，长度=len(p)-m（若m>=len(p)则为空）', params:['系数（降幂排列）','Order of derivative (1=first, 2=second, etc.)']},
  polydiv:  { desc:'多项式除法。返回商和余数。类似 numpy.polydiv。' , ret:'一维FLOAT64商系数数组', params:['被除数系数','Divisor coefficients','Output: remainder (set to NULL if exact division)']},
  polyint:  { desc:'多项式积分。m 指定积分阶数。类似 numpy.polyint。' , ret:'一维FLOAT64数组，长度=len(p)+m', params:['系数','Order of integration']},
  polymul:  { desc:'多项式乘法。系数卷积。类似 numpy.polymul。' , notes:'等价于 convolve(a,b)。两个 n 次和 m 次多项式乘积为 n+m 次。', ret:'一维FLOAT64数组，长度=len(a)+len(b)-1', params:['第一个多项式','Second polynomial']},
  polysub:  { desc:'多项式减法。对齐系数后相减。类似 numpy.polysub。' , params:['第一个','Second']},

  // Window (additional)
  bartlett: { desc:'返回 Bartlett（三角）窗，端点为零。类似 numpy.bartlett。' , notes:'最简单的窗函数。旁瓣电平：−26dB。端点为零。', ret:'一维 FLOAT64 数组', params:['窗口长度']},
  blackman: { desc:'返回 Blackman 窗，综合旁瓣抑制较好。类似 numpy.blackman。' , notes:'经典窗中旁瓣抑制最优：−58.1dB。', ret:'一维 FLOAT64 数组', params:['窗口长度']},
  kaiser:   { desc:'返回 Kaiser 窗。beta 控制主瓣-旁瓣权衡，beta=0 等价于矩形窗。类似 numpy.kaiser。' , notes:'对给定旁瓣电平接近最优。使用 I₀ Bessel 函数近似。β=0→矩形窗。', ret:'一维 FLOAT64 数组', params:['窗口长度','Shape parameter (≥ 0). Larger β = narrower main lobe, lower side-lobes']},

  // Signal (additional)
  correlate:{ desc:'一维互相关。支持 same/valid/full 模式。类似 numpy.correlate。' , ret:'FLOAT64 类型的结果数组', params:['第一个数组（展平）','Second array','"full"=all, "valid"=no padding, "same"=same length as a']},

  // I/O
  genfromtxt:{ desc:'从文本文件读取数组，支持缺失值处理。类似 numpy.genfromtxt。' , ret:'FLOAT64数组。缺失/空值返回NaN', params:['文件路径','Delimiter','Lines to skip']},
  readtxt:  { desc:'从文本文件读取数组。支持分隔符、注释、跳过行等参数。类似 numpy.loadtxt。' , ret:'FLOAT64 类型的结果数组', params:['文件路径','Field delimiter (NULL=whitespace)','Comment character (# is common)','Skip first N lines','Max rows to read (0=all)','Column indices (NULL=all)','Minimum output dimensions','If non-zero, transpose result','Output type']},
  savetxt:  { desc:'将数组保存为文本文件。可指定分隔符。类似 numpy.savetxt。' , ret:'成功返回0', params:['输出路径','Array to save (≤2-D)','Delimiter (NULL=space)']},
  savez:    { desc:'将多个数组保存为 NPZ 压缩归档文件。需要 libzip。类似 numpy.savez。' , notes:'编译链接时需要 libzip(-lzip)。没有 libzip 时返回错误。', ret:'成功返回0', params:['输出.npz路径','数组指针的数组','Array of names for each array','Number of arrays']},
};
