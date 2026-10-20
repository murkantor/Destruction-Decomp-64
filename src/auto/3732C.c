#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f r[3]; } Mtx3f;
typedef struct {
    u8 pad[0x2C];
    u8 *x2C;
    u8 pad30[0xC4 - 0x30];
    f32 xC4;
} WheelSub;
typedef struct {
    WheelSub sub;
    u8 padC8[0xE0 - 0xC8];
    f32 xE0;
    f32 xE4;
    f32 xE8;
    u8 padEC[0x120 - 0xEC];
    f32 x120;
    u8 pad124[0x194 - 0x124];
    Vec3f x194;
    f32 x1A0;
    f32 x1A4;
    u8 pad1A8[4];
    u8 x1AC;
    u8 pad1AD[3];
    f32 x1B0;
    s32 x1B4;
    f32 x1B8;
    u8 pad1BC[0x1BC - 0x1BC];
} Wheel;
typedef struct {
    u8 pad[0x2C];
    u8 *x2C;
    u8 pad30[0x1F8 - 0x30];
    f32 x1F8;
    u8 pad1FC[0x29C - 0x1FC];
    Vec3f x29C;
    Vec3f x2A8;
    Mtx3f x2B4;
    u8 pad2D8[0x364 - 0x2D8];
    Wheel wheels[4];
    u8 padA54[0xB84 - 0x364 - 4 * 0x1BC];
    f32 xB84;
    f32 xB88;
    f32 xB8C;
    u8 padB90[0xBD4 - 0xB90];
    f32 xBD4;
    f32 xBD8;
    s32 xBDC;
} Car;

extern void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
extern void Vec3f_Scale(Vec3f *, Vec3f *, f32);
extern void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
extern f32 Car_GetDriveTorque(Car *);
extern void Math_SinCos(f32, f32 *, f32 *);
extern f64 pow(f64, f64);
extern f32 fabsf(f32);
extern f64 fabs(f64);

static inline s32 gtd(f64 a, f64 b) {
    return a > b;
}

static inline f32 ramp(f32 x, f32 lo, f32 hi) {
    if (x <= lo) {
        return 0.0f;
    }
    if (x >= hi) {
        return 1.0f;
    }
    return (x - lo) / (hi - lo);
}

static inline f32 ramp0(f32 x, f32 hi) {
    if (x <= 0.0f) {
        return 0.0f;
    }
    if (x >= hi) {
        return 1.0f;
    }
    return x / hi;
}

static inline f32 tireF(f32 v, f32 a, f32 b, f32 load) {
    f32 r;
    if (v > 5.0f) {
        r = -a * load;
    } else if (v < -5.0f) {
        r = a * load;
    } else {
        r = v * -0.2f * b * load;
    }
    return r;
}

Vec3f Wheel_ComputeTireForce(Car *car, Wheel *w, f32 load, Vec3f *n, f32 *out, s32 idx) {
    u8 *p = car->x2C;
    Vec3f N = w->x194;
    Vec3f fwd;
    Vec3f side;
    Vec3f vel;
    Vec3f v;
    Vec3f vt;
    Vec3f res = { 0.0f, 0.0f, 0.0f };
    f32 dot;
    f32 radius;
    f32 invR;
    u8 p3;
    u8 front;
    u8 rear;
    WheelSub *ws;
    Mtx3f *m;
    Vec3f *cv;
    u8 p0;
    u8 p1;
    u8 p2;
    f32 c, s;
    f32 sp2;
    f32 lat;
    f32 lon;
    f32 fl;
    f32 ft;
    f32 fs;
    f32 drive;
    f32 a;
    f32 b;
    f32 k;
    f32 d;
    f32 al;
    f64 dl;
    u8 slipPad; /* never assigned, see note */

    radius = w->x120;
    invR = 1.0f / radius;
    ws = &w->sub;
    cv = &car->x29C;
    m = &car->x2B4;
    c = w->xE0;
    s = w->xE8;
    if (idx == 0 || idx == 2) {
        front = 1;
        rear = 0;
    } else {
        front = 0;
        rear = 1;
    }
    p2 = 0;
    if (idx >= 2) {
        p2 = p[2];
    }
    p0 = p[0];
    p1 = p[1];
    p3 = p[3];
    fwd.x = c * m->r[0].x + s * m->r[2].x;
    fwd.y = c * m->r[0].y + s * m->r[2].y;
    fwd.z = c * m->r[0].z + s * m->r[2].z;
    side.x = s * m->r[0].x - c * m->r[2].x;
    side.y = s * m->r[0].y - c * m->r[2].y;
    side.z = s * m->r[0].z - c * m->r[2].z;
    Vec3f_Cross(&v, &car->x2A8, n);
    v.x += cv->x;
    v.y += cv->y;
    v.z += cv->z;
    d = v.x * N.x + v.y * N.y + v.z * N.z;
    vt.x = v.x - d * N.x;
    vt.y = v.y - d * N.y;
    vt.z = v.z - d * N.z;
    k = radius * ws->xC4;
    vel.x = k * side.x + vt.x;
    vel.y = k * side.y + vt.y;
    vel.z = k * side.z + vt.z;
    sp2 = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
    if (sp2 > 80.0f) {
        w->x1AC = 1;
        w->x1B4 = 2;
        w->x1B0 = ramp(sp2, 80.0f, 160.0f);
    } else {
        w->x1AC = 0;
    }
    if (p2) {
        sp2 = vt.x * vt.x + vt.y * vt.y + vt.z * vt.z;
        if (sp2 > 1.0) {
            w->x1AC = 1;
            w->x1B4 = 1;
            w->x1B0 = ramp(sp2, 1.0f, 120.0f);
        }
    }
    lat = vel.x * fwd.x + vel.y * fwd.y + vel.z * fwd.z;
    lon = vel.x * side.x + vel.y * side.y + vel.z * side.z;
    load = load * N.y;
    fl = tireF(lon, w->x1A4, w->x1A0, load);
    if (p2) {
        a = w->x1A4 * 0.55;
        b = w->x1A0 * 0.55;
        ft = tireF(lat, a, b, load);
    } else if (idx >= 2) {
        f32 g = w->x1B8 * 0.45 + 0.55;
        ft = tireF(lat, g * w->x1A4, g * w->x1A0, load);
    } else {
        ft = tireF(lat, w->x1A4, w->x1A0, load);
    }
    fs = cv->x * m->r[0].x + cv->y * m->r[0].y + cv->z * m->r[0].z;
    if ((p3 && fs > 3.0) || (p0 && !p3 && fs < -3.0)) {
        w->x1AC = 1;
        w->x1B4 = slipPad >> 8; /* always 0, see note */
        w->x1B0 = ramp0(fabsf(fs), 30.0f);
        fl *= 1.3;
        ft *= 1.3;
        p0 = 0;
        p1 = 1;
    }
    fl *= car->xB84;
    ft *= car->xB84;
    if (p0 && !p2) {
        f32 q;
        f32 fx;
        f32 aq;
        f32 lim;
        drive = Car_GetDriveTorque(car);
        if (p1) {
            drive *= 0.1;
        }
        q = drive * invR;
        lim = w->x1A0 * load;
        aq = fabsf(q);
        Vec3f_Dot(&dot, &vt, &vt);
        if ((lim < aq || fabs(lon) > 8.0 || w->x1AC) && dot < 1225.0) {
            if (!w->x1AC) {
                w->x1AC = 1;
                w->x1B4 = 0;
                w->x1B0 = ramp0(aq, 6000.0f);
            }
            *out = *out - drive + fl * radius;
            fx = fl;
        } else {
            *out = 0.0f;
            k = -invR * (vt.x * side.x + vt.y * side.y + vt.z * side.z);
            ws->xC4 = k;
            fx = q;
        }
        res.x = fx * side.x + ft * fwd.x;
        res.y = fx * side.y + ft * fwd.y;
        res.z = fx * side.z + ft * fwd.z;
    } else {
        if (p2 || (p1 && fabs(ws->xC4) < 10.0)) {
            *out = 0.0f;
            ws->xC4 = 0.0f;
        } else if (p1) {
            *out = 0.0f;
            if (ws->xC4 > *out) {
                ws->xC4 = ws->xC4 - car->xB88 * 1.4;
            } else {
                ws->xC4 = ws->xC4 + car->xB88 * 1.4;
            }
        } else {
            f32 h;
            *out = 0.0f;
            k = -invR * (vt.x * side.x + vt.y * side.y + vt.z * side.z);
            ws->xC4 = k;
            h = car->xB8C * 0.6;
            if (ws->xC4 > h * 2.0) {
                ws->xC4 = ws->xC4 - h;
            } else if (ws->xC4 < -h * 2.0) {
                ws->xC4 = ws->xC4 + h;
            } else if (N.y > 0.98) {
                ws->xC4 = ws->xC4 * 0.95;
            }
        }
        res.x = fl * side.x + ft * fwd.x;
        res.y = fl * side.y + ft * fwd.y;
        res.z = fl * side.z + ft * fwd.z;
    }
    dl = lat;
    al = fabsf(lat);
    if (!w->x1AC && (al > 7.0 || (rear & gtd(dl, 4.0)) || (front & gtd(-4.0, dl)))) {
        if (!w->x1AC) {
            w->x1B4 = 2;
            w->x1AC = 1;
            w->x1B0 = ramp0(al, 8.0f);
        }
    } else if (idx >= 2) {
        Wheel *o = (idx == 2) ? &car->wheels[1] : &car->wheels[0];
        if (o->x1AC && o->x1B4 == 2 && !w->x1AC) {
            w->x1AC = 1;
            w->x1B4 = 2;
            w->x1B0 = o->x1B0;
        }
    }
    if (car->xBDC) {
        if (car->xBD4 < 0.51) {
            Vec3f_Scale(&res, &res, car->xBD4);
            w->x1AC = 1;
            w->x1B4 = 0;
            w->x1B0 = 1.0f;
            if (car->xBD4 < 0.35 && idx >= 2) {
                Vec3f t;
                f32 sn;
                f32 cs;
                f32 r = car->x1F8 * 1.2;
                t = car->x2B4.r[2];
                Math_SinCos((car->xBD8 + 0.1) * 4.084070445, &sn, &cs);
                Vec3f_Scale(&t, &t, cs * r);
                Vec3f_Add(&res, &res, &t);
            }
        } else {
            f32 r;
            if (car->xBD8 < 2.0) {
                w->x1AC = 1;
                w->x1B4 = 0;
                w->x1B0 = 1.0f;
            }
            r = pow(0.75, car->xBD8) * 1.2 + 1.0;
            Vec3f_Scale(&res, &res, r);
            *out = *out * r;
        }
    }
    return res;
}
/* agent54: code now identical except the frame: ours is 0x128, original 0x130 (8 unused bytes at
 * 208(sp), above the last spill slot rear@207). All other words match (46 words differ = the
 * frame-relative offsets). Fixes this pass: tireF inline uses one local r and a single return
 * (gives the mov.s from f0); res in the drive branch goes through fx (fl / q) so q lands in s1;
 * gtd() inline for the (dl > 4.0) & rear store-flags; al = fabsf(lat) and dl = lat before the if;
 * spin value of xC4 reuses k, and the threshold uses a new h; t = r[2] (was r[1]);
 * r = x1F8 * 1.2 before Math_SinCos; w->x1B4 = 2 in the o-copy branch; ws->xC4 > *out.
 * Unused char/s32 arrays shift the spill slots (locals sit below them), so the 8 bytes are
 * probably a spill slot reload allocated and then did not use, or a caller-save slot. */
/* agent58: unchanged code. Findings on the 8-byte hole (208..215, above the last pseudo slot):
 * reload allocates frame space after the pseudo slots only for (a) alter_reg slots of pseudos
 * numbered after rear or spilled later, (b) caller-save areas (setup_save_areas), (c) secondary
 * memory (MODE_INT moves FP<->GR). Toy tests confirm (b) leaves an unused slot when a pseudo has a
 * stale reg_n_calls_crossed (flow count not updated after combine/sched) and sits in a call-clobbered
 * reg: e.g. an f32 var set to 0.0f before a call and stored after it (combine turns the store into
 * sw $0 across the call), or a copy/const set that sched1 moves past a call (multi-block pseudos keep
 * the old count). That is how Car_ApplySuspensionForces got its +8 (w2 = w before the calls). For this function
 * the pseudo would need >= 5 refs and to conflict with all of f20-f30 (only true around Vec3f_Dot)
 * or all s-regs; no visible value fits, and ~70 constant/temp insertions (exp/, bf1.py) did not
 * produce vars=200 with identical code. */
/* agent60: unchanged code (frame 8 short). Built a detector (build/agent60/bf/det.py: compile with
 * -dl -dg, list pseudos with crosses>0 in call-clobbered regs = what makes setup_save_areas add a slot)
 * and brute-forced with it: bf.py (V_2 = V copies before every call, all vars, all positions, uses after
 * the call renamed), bf2.py (constant temps cT = <literal> before each call; moving each call +-3
 * lines), bf3.py (every simple statement moved +-3 lines). No variant gives vars=200 with identical
 * code. Findings: in our code the only global pseudo in a call-clobbered reg with >= 5 refs is k (f12);
 * fx (f14) has exactly 5 refs and would take the caller-save path if its live range covered the
 * Vec3f_Dot branch test (all f20-f30 live there), e.g. 'fx = q' before Vec3f_Dot (wh_a1-style) gives
 * the right frame but then fx coalesces with q and lat moves to f14 with real saves. The original
 * keeps both fx assignments inside the branches (mov.s f14,f28 / mtc1 s1,f14), so a default fx before
 * the call does not fit. Copies of idx/p2 give vars=200 but add a real spill slot. */
/* agent64: unchanged code (frame 8 short). The hole at 0xD0 is exactly one more 8-byte slot after the
 * six pseudo spill slots 0xA0..0xC8 (retptr, 2 floats, 3 u8s), so it is either a 7th spilled pseudo
 * numbered after rear with stale reg_n_refs and no remaining insns, or a caller-save area.
 * GCC 2.7 combine zeroes reg_n_refs of a merged-away pseudo only when its reg_n_sets drops to 0 AND it
 * is not live at function start (combine.c ~2313), so a pseudo with a second set elsewhere, or one
 * used uninitialised on some path, keeps stale refs. Tried here: fx = aq / fx = lim before Vec3f_Dot
 * with the copy used in the following compare (fx goes to f22, not merged), fx = fl before Vec3f_Dot
 * (code moves), selq() inline for fx = q, one/zero/flag temps across the first Vec3f_Scale. None
 * gave vars=200 with identical code. Caller-save with real liveness always inserts save code, so a
 * caller-save slot needs a set that sched1 moves past the call (pseudo counted as crossing by flow). */
/* agent66: unchanged code (frame 8 short). Tried: (1) hard-register locals: every local bound to every
 * FP/GPR (bf/bf.py hard): q asm("$17") is neutral (identical code), all others change code or add a
 * real spill; toy test shows a 2.7.2 hard-reg local live across a call is simply clobbered (no
 * caller-save slot), since setup_save_areas only looks at pseudos. (2) dead branches
 * 'if (c) { t_++; t_ *= 3; }' at every statement x 7 conds (bf.py dead): vars=200 only via real spills.
 * (3) fx = <q|fl|ft|drive|aq|lim|lat|lon|load|0.0f|invR|radius> at every line of the drive block,
 * with/without the branch copies and with q in $17 (bf2.py): vars=200 only when fx becomes a real
 * spill (slot 0xD0 used). With q in $17 and 'fx = q' before Vec3f_Dot, sched1 moves the copy after the
 * call (after the compare), so fx gets f20, not a caller-save reg. (4) all pairs of +-3 line moves in
 * the drive block (bf3.py, 4753 variants): none with vars=200 and identical code.
 * Analysis: the slot must be a caller-save area: a pseudo with reg_n_calls_crossed>0 (stale: kept by
 * sched for multi-block pseudos, or combine moving a constant across a call) in a call-used reg, not
 * live at any call. For FP it must conflict with all f20-f30, which only happens in the Vec3f_Dot
 * block between lim's set and the lim<aq compare; ints always have s5/s7 free there. Every code-free
 * way to be live there that I found (copy after the call, no-op copy deleted by jump2) needs code that
 * the original does not have. Scripts: build/agent66/bf/, build/agent66/t/run.sh, df.sh. */
/* agent70: unchanged code (frame still 0x128 vs 0x130). Findings:
 * - mips.c compute_frame_size rounds var_size to 8, so get_frame_size() of 196 is enough: one 4-byte
 *   slot works. Original saves s0 at 0xD8 (ours 0xD0), so locals+slots end at 0xD0 in ours and at
 *   0xD4..0xD8 in the original; the extra space is ABOVE the six spill slots (allocated during reload).
 * - Reload-time slots: alter_reg spill slots (always 8, align -1), caller-save areas (4 bytes: MOVE_MAX=4,
 *   so regno_save_mode[r][1] is SImode for a GPR and SFmode/SImode for an FP reg), secondary memory
 *   (SImode/QImode moves GR<->FP, 4 bytes), and a local pseudo kicked out of a spill reg (t0/f18/f19)
 *   by spill_hard_reg: local pseudos have no allocno, so retry_global_alloc does nothing and they go to
 *   a new slot at the end; if all their uses are inherited, delete_output_reload removes the store and
 *   the code is identical. Seen working: 'drive_c' variant kicks pseudo 170 (N.z) out of f16.
 * - For the kick route the original needs a 10th live FP local in the post-Vec3f_Cross block (f0-f16
 *   are all used there) whose value ends in f18: only the radius reload fits, i.e. a copy of radius that
 *   survives CSE (plain copies, struct wrappers, block copies are all propagated away).
 * - GPR caller-save route: all 9 s-regs are live between lon's set (mfc1 s4) and the fs dot product;
 *   no multi-block int in a call-clobbered reg exists there in our code.
 * - bf/bf.py (copy per statement, block / do-while forms) and bf/bf2.py (V2 = V before each call,
 *   uses renamed over 1..60 following lines, for Cross/GetDriveTorque/Dot/Scale): no vars=200 with
 *   identical code. Closest: idx2 = idx before Vec3f_Cross used in 'else if (idx2 >= 2)': its slot
 *   lands exactly at 0xD0 but adds lw/sw. */
/* agent72: MATCH. The missing 8 bytes of frame (0x128 vs 0x130) are a spill slot for a pseudo
 * that leaves no code: slipPad is never assigned, and its only use (slipPad >> 8, an unsigned
 * byte shifted out) is folded to 0 by combine, which runs after flow. combine does not reset
 * reg_n_refs of a pseudo that is live at function entry (uninitialised), so the pseudo keeps
 * 1 ref, no insns, and a stale live range from entry to the fs branch. In global-alloc that
 * range conflicts with every callee-saved GPR (it also "crosses" the calls), caller-save is not
 * profitable (1 ref), so it stays unallocated and reload's alter_reg gives it an 8-byte slot
 * above the six real spill slots (0xD0). The fold must be one CSE cannot do (CSE associates
 * same-opcode chains like (x & 1) & 2, and folds (x | c) == 0), e.g. u8 >> 8, u16 >> 16,
 * (x & 0xFF) >> 8, (u8)x >> 8, ((x | 1) & 1) - 1, in any block after lon is set (the fs branch
 * or the Vec3f_Dot branch both work). A plain copy, hard-register local, dead branch, or
 * caller-save/secondary-memory route cannot produce this slot: a local in $f18 would need to
 * span the dot-product peak with one use (the k multiply), which the backward list scheduler
 * places next to its consumer, and k would then prefer $f18 via set_preference. */
