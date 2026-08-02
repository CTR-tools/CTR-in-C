/*
 * RenderBucket_InitDepthGTE @ 0x8006AE74  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Behavior-exact: Branch/Stack/Register 0 (score 60 — sole residual is the last ctc2 landing
 * in the jr delay slot in retail vs before jr here; functionally identical; gcc can't reschedule
 * inside a volatile asm block). Sets the GTE depth-cue control regs (27/28=0, 29=0x555, 30=0x400).
 *
 * Phase B (saphi game/RenderBucket/RenderBucket_08_InitDepthGTE.c): CLEAN (identical 4x CTC2).
 */
void RenderBucket_InitDepthGTE(void)
{
    __asm__ __volatile__(
        "ctc2 $0, $27\n"
        "ctc2 $0, $28\n"
        "li   $3, 0x555\n"
        "ctc2 $3, $29\n"
        "li   $3, 0x400\n"
        "ctc2 $3, $30\n"
        ::: "$3");
}
