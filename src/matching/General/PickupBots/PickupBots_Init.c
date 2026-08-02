/*
 * PickupBots_Init @ 0x80040850  (vs SCUS_944.26.exe / NTSC-U 926)
 *
 * Byte-exact: score 0 (Branch/Stack/Register 0).
 * Sets g_pBossWeaponMeta = g_apBossWeaponMeta[metaDataLEV[gGT->levelID].hubID] for the current
 * hub world (hub forced 0 for Oxide Station, skipped if hubID < 0). Key lever: hub is a SIGNED
 * short sign-extended at use (after the levelID==0xD branch), not shifted early.
 *
 * Phase B (saphi): CLEAN
 */
struct MetaDataLEV { unsigned short hubID; char pad[0x16]; };  /* stride 0x18 */
extern char *D_8008D2AC;                /* gGT */
extern struct MetaDataLEV D_80083A80[]; /* metaDataLEV */
extern void *D_800859D0[];              /* g_apBossWeaponMeta */
void *D_8008D8E8 = 0;                    /* g_pBossWeaponMeta (gp+0x97C) */
void PickupBots_Init(void)
{
    int levelID;
    short hub;
    levelID = *(int *)(D_8008D2AC + 0x1A10);
    hub = D_80083A80[levelID].hubID;
    if (levelID == 0xd) {
        hub = 0;
    }
    if (hub >= 0) {
        D_8008D8E8 = D_800859D0[hub];
    }
}
