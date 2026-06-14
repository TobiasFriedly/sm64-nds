#ifndef OBJECT_FIELDS_H
#define OBJECT_FIELDS_H

#ifdef OBJECT_FIELDS_INDEX_DIRECTLY
#define OBJECT_FIELD_U32(index)           index
#define OBJECT_FIELD_S32(index)           index
#define OBJECT_FIELD_S16(index, subIndex) index
#define OBJECT_FIELD_F32(index)           index
#define OBJECT_FIELD_S16P(index)          index
#define OBJECT_FIELD_S32P(index)          index
#define OBJECT_FIELD_ANIMS(index)         index
#define OBJECT_FIELD_WAYPOINT(index)      index
#define OBJECT_FIELD_CHAIN_SEGMENT(index) index
#define OBJECT_FIELD_OBJ(index)           index
#define OBJECT_FIELD_SURFACE(index)       index
#define OBJECT_FIELD_VPTR(index)          index
#define OBJECT_FIELD_CVPTR(index)         index
#else
#define OBJECT_FIELD_U32(index)           rawData.asU32[index]
#define OBJECT_FIELD_S32(index)           rawData.asS32[index]
#define OBJECT_FIELD_S16(index, subIndex) rawData.asS16[index][subIndex]
#define OBJECT_FIELD_F32(index)           rawData.asF32[index]
#if !IS_64_BIT
#define OBJECT_FIELD_S16P(index)          rawData.asS16P[index]
#define OBJECT_FIELD_S32P(index)          rawData.asS32P[index]
#define OBJECT_FIELD_ANIMS(index)         rawData.asAnims[index]
#define OBJECT_FIELD_WAYPOINT(index)      rawData.asWaypoint[index]
#define OBJECT_FIELD_CHAIN_SEGMENT(index) rawData.asChainSegment[index]
#define OBJECT_FIELD_OBJ(index)           rawData.asObject[index]
#define OBJECT_FIELD_SURFACE(index)       rawData.asSurface[index]
#define OBJECT_FIELD_VPTR(index)          rawData.asVoidPtr[index]
#define OBJECT_FIELD_CVPTR(index)         rawData.asConstVoidPtr[index]
#else
#define OBJECT_FIELD_S16P(index)          ptrData.asS16P[index]
#define OBJECT_FIELD_S32P(index)          ptrData.asS32P[index]
#define OBJECT_FIELD_ANIMS(index)         ptrData.asAnims[index]
#define OBJECT_FIELD_WAYPOINT(index)      ptrData.asWaypoint[index]
#define OBJECT_FIELD_CHAIN_SEGMENT(index) ptrData.asChainSegment[index]
#define OBJECT_FIELD_OBJ(index)           ptrData.asObject[index]
#define OBJECT_FIELD_SURFACE(index)       ptrData.asSurface[index]
#define OBJECT_FIELD_VPTR(index)          ptrData.asVoidPtr[index]
#define OBJECT_FIELD_CVPTR(index)         ptrData.asConstVoidPtr[index]
#endif
#endif

#define  oFlags                      OBJECT_FIELD_U32(0x01)
#define  oDialogResponse             OBJECT_FIELD_S16(0x02, 0)
#define  oDialogState                OBJECT_FIELD_S16(0x02, 1)
#define  oUnk94                      OBJECT_FIELD_U32(0x03)

#define  oIntangibleTimer            OBJECT_FIELD_S32(0x05)
#define  O_POS_INDEX                 0x06
#define  oPosX                       OBJECT_FIELD_F32(O_POS_INDEX + 0)
#define  oPosY                       OBJECT_FIELD_F32(O_POS_INDEX + 1)
#define  oPosZ                       OBJECT_FIELD_F32(O_POS_INDEX + 2)
#define  oVelX                       OBJECT_FIELD_F32(0x09)
#define  oVelY                       OBJECT_FIELD_F32(0x0A)
#define  oVelZ                       OBJECT_FIELD_F32(0x0B)
#define  oForwardVel                 OBJECT_FIELD_F32(0x0C)
#define  oForwardVelS32              OBJECT_FIELD_S32(0x0C)
#define  oLeftVel                    OBJECT_FIELD_F32(0x0D)
#define  oUpVel                      OBJECT_FIELD_F32(0x0E)
#define  O_MOVE_ANGLE_INDEX          0x0F
#define  O_MOVE_ANGLE_PITCH_INDEX    (O_MOVE_ANGLE_INDEX + 0)
#define  O_MOVE_ANGLE_YAW_INDEX      (O_MOVE_ANGLE_INDEX + 1)
#define  O_MOVE_ANGLE_ROLL_INDEX     (O_MOVE_ANGLE_INDEX + 2)
#define  oMoveAnglePitch             OBJECT_FIELD_S32(O_MOVE_ANGLE_PITCH_INDEX)
#define  oMoveAngleYaw               OBJECT_FIELD_S32(O_MOVE_ANGLE_YAW_INDEX)
#define  oMoveAngleRoll              OBJECT_FIELD_S32(O_MOVE_ANGLE_ROLL_INDEX)
#define  O_FACE_ANGLE_INDEX          0x12
#define  O_FACE_ANGLE_PITCH_INDEX    (O_FACE_ANGLE_INDEX + 0)
#define  O_FACE_ANGLE_YAW_INDEX      (O_FACE_ANGLE_INDEX + 1)
#define  O_FACE_ANGLE_ROLL_INDEX     (O_FACE_ANGLE_INDEX + 2)
#define  oFaceAnglePitch             OBJECT_FIELD_S32(O_FACE_ANGLE_PITCH_INDEX)
#define  oFaceAngleYaw               OBJECT_FIELD_S32(O_FACE_ANGLE_YAW_INDEX)
#define  oFaceAngleRoll              OBJECT_FIELD_S32(O_FACE_ANGLE_ROLL_INDEX)
#define  oGraphYOffset               OBJECT_FIELD_F32(0x15)
#define  oActiveParticleFlags        OBJECT_FIELD_U32(0x16)
#define  oGravity                    OBJECT_FIELD_F32(0x17)
#define  oFloorHeight                OBJECT_FIELD_F32(0x18)
#define  oMoveFlags                  OBJECT_FIELD_U32(0x19)
#define  oAnimState                  OBJECT_FIELD_S32(0x1A)

#define  oAngleVelPitch              OBJECT_FIELD_S32(0x23)
#define  oAngleVelYaw                OBJECT_FIELD_S32(0x24)
#define  oAngleVelRoll               OBJECT_FIELD_S32(0x25)
#define  oAnimations                 OBJECT_FIELD_ANIMS(0x26)
#define  oHeldState                  OBJECT_FIELD_U32(0x27)
#define  oWallHitboxRadius           OBJECT_FIELD_F32(0x28)
#define  oDragStrength               OBJECT_FIELD_F32(0x29)
#define  oInteractType               OBJECT_FIELD_U32(0x2A)
#define  oInteractStatus             OBJECT_FIELD_S32(0x2B)
#define  O_PARENT_RELATIVE_POS_INDEX 0x2C
#define  oParentRelativePosX         OBJECT_FIELD_F32(O_PARENT_RELATIVE_POS_INDEX + 0)
#define  oParentRelativePosY         OBJECT_FIELD_F32(O_PARENT_RELATIVE_POS_INDEX + 1)
#define  oParentRelativePosZ         OBJECT_FIELD_F32(O_PARENT_RELATIVE_POS_INDEX + 2)
#define  oBhvParams2ndByte           OBJECT_FIELD_S32(0x2F)

#define  oAction                     OBJECT_FIELD_S32(0x31)
#define  oSubAction                  OBJECT_FIELD_S32(0x32)
#define  oTimer                      OBJECT_FIELD_S32(0x33)
#define  oBounciness                 OBJECT_FIELD_F32(0x34)
#define  oDistanceToMario            OBJECT_FIELD_F32(0x35)
#define  oAngleToMario               OBJECT_FIELD_S32(0x36)
#define  oHomeX                      OBJECT_FIELD_F32(0x37)
#define  oHomeY                      OBJECT_FIELD_F32(0x38)
#define  oHomeZ                      OBJECT_FIELD_F32(0x39)
#define  oFriction                   OBJECT_FIELD_F32(0x3A)
#define  oBuoyancy                   OBJECT_FIELD_F32(0x3B)
#define  oSoundStateID               OBJECT_FIELD_S32(0x3C)
#define  oOpacity                    OBJECT_FIELD_S32(0x3D)
#define  oDamageOrCoinValue          OBJECT_FIELD_S32(0x3E)
#define  oHealth                     OBJECT_FIELD_S32(0x3F)
#define  oBhvParams                  OBJECT_FIELD_S32(0x40)
#define  oPrevAction                 OBJECT_FIELD_S32(0x41)
#define  oInteractionSubtype         OBJECT_FIELD_U32(0x42)
#define  oCollisionDistance          OBJECT_FIELD_F32(0x43)
#define  oNumLootCoins               OBJECT_FIELD_S32(0x44)
#define  oDrawingDistance            OBJECT_FIELD_F32(0x45)
#define  oRoom                       OBJECT_FIELD_S32(0x46)

#define  oUnusedBhvParams            OBJECT_FIELD_U32(0x48)

#define  oWallAngle                  OBJECT_FIELD_S32(0x4B)
#define  oFloorType                  OBJECT_FIELD_S16(0x4C, 0)
#define  oFloorRoom                  OBJECT_FIELD_S16(0x4C, 1)
#define  oAngleToHome                OBJECT_FIELD_S32(0x4D)
#define  oFloor                      OBJECT_FIELD_SURFACE(0x4E)
#define  oDeathSound                 OBJECT_FIELD_S32(0x4F)

#define  oPathedStartWaypoint     OBJECT_FIELD_WAYPOINT(0x1D)
#define  oPathedPrevWaypoint      OBJECT_FIELD_WAYPOINT(0x1E)
#define  oPathedPrevWaypointFlags OBJECT_FIELD_S32(0x1F)
#define  oPathedTargetPitch       OBJECT_FIELD_S32(0x20)
#define  oPathedTargetYaw         OBJECT_FIELD_S32(0x21)

#define  oMacroUnk108 OBJECT_FIELD_F32(0x20)
#define  oMacroUnk10C OBJECT_FIELD_F32(0x21)
#define  oMacroUnk110 OBJECT_FIELD_F32(0x22)

#define  oMarioParticleFlags    OBJECT_FIELD_S32(0x1B)
#define  oMarioPoleUnk108       OBJECT_FIELD_S32(0x20)
#define  oMarioReadingSignDYaw  OBJECT_FIELD_S32(0x20)
#define  oMarioPoleYawVel       OBJECT_FIELD_S32(0x21)
#define  oMarioCannonObjectYaw  OBJECT_FIELD_S32(0x21)
#define  oMarioTornadoYawVel    OBJECT_FIELD_S32(0x21)
#define  oMarioReadingSignDPosX OBJECT_FIELD_F32(0x21)
#define  oMarioPolePos          OBJECT_FIELD_F32(0x22)
#define  oMarioCannonInputYaw   OBJECT_FIELD_S32(0x22)
#define  oMarioTornadoPosY      OBJECT_FIELD_F32(0x22)
#define  oMarioReadingSignDPosZ OBJECT_FIELD_F32(0x22)
#define  oMarioWhirlpoolPosY    OBJECT_FIELD_F32(0x22)
#define  oMarioBurnTimer        OBJECT_FIELD_S32(0x22)
#define  oMarioLongJumpIsSlow   OBJECT_FIELD_S32(0x22)
#define  oMarioSteepJumpYaw     OBJECT_FIELD_S32(0x22)
#define  oMarioWalkingPitch     OBJECT_FIELD_S32(0x22)

#define  oHidden1UpNumTouchedTriggers OBJECT_FIELD_S32(0x1B)

#define  oActivatedBackAndForthPlatformMaxOffset    OBJECT_FIELD_F32(0x1B)
#define  oActivatedBackAndForthPlatformOffset       OBJECT_FIELD_F32(0x1C)
#define  oActivatedBackAndForthPlatformVel          OBJECT_FIELD_F32(0x1D)
#define  oActivatedBackAndForthPlatformCountdown    OBJECT_FIELD_S32(0x1E)
#define  oActivatedBackAndForthPlatformStartYaw     OBJECT_FIELD_S32(0x1F)
#define  oActivatedBackAndForthPlatformVertical     OBJECT_FIELD_S32(0x20)
#define  oActivatedBackAndForthPlatformFlipRotation OBJECT_FIELD_S32(0x21)

#define  oAmpRadiusOfRotation OBJECT_FIELD_F32(0x1B)
#define  oAmpYPhase           OBJECT_FIELD_S32(0x1C)

#define  oHomingAmpLockedOn OBJECT_FIELD_S32(0x1B)
#define  oHomingAmpAvgY     OBJECT_FIELD_F32(0x1D)

#define  oArrowLiftDisplacement OBJECT_FIELD_F32(0x1B)
#define  oArrowLiftUnk100       OBJECT_FIELD_S32(0x1E)

#define  oBackAndForthPlatformDirection  OBJECT_FIELD_F32(0x1B)
#define  oBackAndForthPlatformPathLength OBJECT_FIELD_F32(0x1C)
#define  oBackAndForthPlatformDistance   OBJECT_FIELD_F32(0x1D)
#define  oBackAndForthPlatformVel        OBJECT_FIELD_F32(0x1E)

#define  oBirdSpeed       OBJECT_FIELD_F32(0x1B)
#define  oBirdTargetPitch OBJECT_FIELD_S32(0x1C)
#define  oBirdTargetYaw   OBJECT_FIELD_S32(0x1D)

#define  oBirdChirpChirpUnkF4 OBJECT_FIELD_S32(0x1B)

#define  oEndBirdUnk104 OBJECT_FIELD_F32(0x1F)

#define  oHiddenBlueCoinSwitch OBJECT_FIELD_OBJ(0x1C)

#define  oBobombBlinkTimer OBJECT_FIELD_S32(0x1B)
#define  oBobombFuseLit    OBJECT_FIELD_S32(0x1C)
#define  oBobombFuseTimer  OBJECT_FIELD_S32(0x1D)

#define  oBobombBuddyBlinkTimer       OBJECT_FIELD_S32(0x1B)
#define  oBobombBuddyHasTalkedToMario OBJECT_FIELD_S32(0x1C)
#define  oBobombBuddyRole             OBJECT_FIELD_S32(0x1D)
#define  oBobombBuddyCannonStatus     OBJECT_FIELD_S32(0x1E)
#define  oBobombBuddyPosXCopy         OBJECT_FIELD_F32(0x20)
#define  oBobombBuddyPosYCopy         OBJECT_FIELD_F32(0x21)
#define  oBobombBuddyPosZCopy         OBJECT_FIELD_F32(0x22)

#define  oBobombExpBubGfxScaleFacX OBJECT_FIELD_S32(0x1D)
#define  oBobombExpBubGfxScaleFacY OBJECT_FIELD_S32(0x1E)
#define  oBobombExpBubGfxExpRateX  OBJECT_FIELD_S32(0x1F)
#define  oBobombExpBubGfxExpRateY  OBJECT_FIELD_S32(0x20)

#define  oSmallBompInitX OBJECT_FIELD_F32(0x1E)

#define  oBooDeathStatus           OBJECT_FIELD_S32(0x00)
#define  oBooTargetOpacity         OBJECT_FIELD_S32(0x1B)
#define  oBooBaseScale             OBJECT_FIELD_F32(0x1C)
#define  oBooOscillationTimer      OBJECT_FIELD_S32(0x1D)
#define  oBooMoveYawDuringHit      OBJECT_FIELD_S32(0x1E)
#define  oBooMoveYawBeforeHit      OBJECT_FIELD_F32(0x1F)
#define  oBooParentBigBoo          OBJECT_FIELD_OBJ(0x20)
#define  oBooNegatedAggressiveness OBJECT_FIELD_F32(0x21)
#define  oBooInitialMoveYaw        OBJECT_FIELD_S32(0x22)
#define  oBooTurningSpeed          OBJECT_FIELD_S16(0x4A, 0)

#define  oBigBooNumMinionBoosKilled OBJECT_FIELD_S32(0x49)

#define  oBookendUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oBookendUnkF8 OBJECT_FIELD_S32(0x1C)

#define  oBookSwitchUnkF4 OBJECT_FIELD_F32(0x1B)

#define  oBookSwitchManagerUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oBookSwitchManagerUnkF8 OBJECT_FIELD_S32(0x1C)

#define  oHauntedBookshelfShouldOpen OBJECT_FIELD_S32(0x00)

#define  oBouncingFireBallUnkF4 OBJECT_FIELD_S32(0x1B)

#define  oBowlingBallTargetYaw OBJECT_FIELD_S32(0x1B)

#define  oBBallSpawnerMaxSpawnDist OBJECT_FIELD_F32(0x1B)
#define  oBBallSpawnerSpawnOdds    OBJECT_FIELD_F32(0x1C)
#define  oBBallSpawnerPeriodMinus1 OBJECT_FIELD_S32(0x1D)

#define  oBowserCamAct          OBJECT_FIELD_S32(0x00)
#define  oBowserStatus          OBJECT_FIELD_S32(0x1B)
#define  oBowserTimer           OBJECT_FIELD_S32(0x1C)
#define  oBowserDistToCenter    OBJECT_FIELD_F32(0x1D)
#define  oBowserBitSJustJump    OBJECT_FIELD_S16(0x1F, 1)
#define  oBowserRandSplitFloor  OBJECT_FIELD_S16(0x20, 0)
#define  oBowserHeldAnglePitch  OBJECT_FIELD_S16(0x20, 1)
#define  oBowserHeldAngleVelYaw OBJECT_FIELD_S16(0x21, 0)
#define  oBowserGrabbedStatus   OBJECT_FIELD_S16(0x21, 1)
#define  oBowserIsReacting      OBJECT_FIELD_S16(0x22, 0)
#define  oBowserAngleToCenter   OBJECT_FIELD_S16(0x22, 1)
#define  oBowserTargetOpacity   OBJECT_FIELD_S16(0x49, 0)
#define  oBowserEyesTimer       OBJECT_FIELD_S16(0x49, 1)
#define  oBowserEyesShut        OBJECT_FIELD_S16(0x4A, 0)
#define  oBowserRainbowLight    OBJECT_FIELD_S16(0x4A, 1)

#define  oBowserShockWaveScale OBJECT_FIELD_F32(0x1B)

#define  oBlackSmokeBowserUnkF4 OBJECT_FIELD_F32(0x1B)

#define  oBowserKeyScale OBJECT_FIELD_F32(0x1B)

#define  oBowserPuzzleCompletionFlags OBJECT_FIELD_S32(0x1B)

#define  oBowserPuzzlePieceOffsetX                  OBJECT_FIELD_F32(0x1D)
#define  oBowserPuzzlePieceOffsetY                  OBJECT_FIELD_F32(0x1E)
#define  oBowserPuzzlePieceOffsetZ                  OBJECT_FIELD_F32(0x1F)
#define  oBowserPuzzlePieceContinuePerformingAction OBJECT_FIELD_S32(0x20)
#define  oBowserPuzzlePieceActionList               OBJECT_FIELD_VPTR(0x21)
#define  oBowserPuzzlePieceNextAction               OBJECT_FIELD_VPTR(0x22)

#define  oBubbaUnkF4  OBJECT_FIELD_F32(0x1B)
#define  oBubbaUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oBubbaUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oBubbaUnk100 OBJECT_FIELD_S32(0x1E)
#define  oBubbaUnk104 OBJECT_FIELD_S32(0x1F)
#define  oBubbaUnk108 OBJECT_FIELD_F32(0x20)
#define  oBubbaUnk10C OBJECT_FIELD_F32(0x21)
#define  oBubbaUnk1AC OBJECT_FIELD_S16(0x49, 0)
#define  oBubbaUnk1AE OBJECT_FIELD_S16(0x49, 1)
#define  oBubbaUnk1B0 OBJECT_FIELD_S16(0x4A, 0)
#define  oBubbaUnk1B2 OBJECT_FIELD_S16(0x4A, 1)

#define  oBulletBillInitialMoveYaw OBJECT_FIELD_S32(0x1C)

#define  oBullySubtype                   OBJECT_FIELD_S32(0x1B)
#define  oBullyPrevX                     OBJECT_FIELD_F32(0x1C)
#define  oBullyPrevY                     OBJECT_FIELD_F32(0x1D)
#define  oBullyPrevZ                     OBJECT_FIELD_F32(0x1E)
#define  oBullyKBTimerAndMinionKOCounter OBJECT_FIELD_S32(0x1F)
#define  oBullyMarioCollisionAngle       OBJECT_FIELD_S32(0x20)

#define  oButterflyYPhase OBJECT_FIELD_S32(0x1B)

#define  oTripletButterflyScale             OBJECT_FIELD_F32(0x1B)
#define  oTripletButterflySpeed             OBJECT_FIELD_F32(0x1C)
#define  oTripletButterflyBaseYaw           OBJECT_FIELD_F32(0x1D)
#define  oTripletButterflyTargetPitch       OBJECT_FIELD_S32(0x1E)
#define  oTripletButterflyTargetYaw         OBJECT_FIELD_S32(0x1F)
#define  oTripletButterflyType              OBJECT_FIELD_S32(0x20)
#define  oTripletButterflyModel             OBJECT_FIELD_S32(0x21)
#define  oTripletButterflySelectedButterfly OBJECT_FIELD_S32(0x22)
#define  oTripletButterflyScalePhase        OBJECT_FIELD_S32(0x49)

#define  oCannonUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oCannonUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oCannonUnk10C OBJECT_FIELD_S32(0x21)

#define  oCapUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oCapUnkF8 OBJECT_FIELD_S32(0x1C)

#define  oChainChompSegments                     OBJECT_FIELD_CHAIN_SEGMENT(0x1B)
#define  oChainChompMaxDistFromPivotPerChainPart OBJECT_FIELD_F32(0x1C)
#define  oChainChompMaxDistBetweenChainParts     OBJECT_FIELD_F32(0x1D)
#define  oChainChompDistToPivot                  OBJECT_FIELD_F32(0x1E)
#define  oChainChompUnk104                       OBJECT_FIELD_F32(0x1F)
#define  oChainChompRestrictedByChain            OBJECT_FIELD_S32(0x20)
#define  oChainChompTargetPitch                  OBJECT_FIELD_S32(0x21)
#define  oChainChompNumLunges                    OBJECT_FIELD_S32(0x22)
#define  oChainChompReleaseStatus                OBJECT_FIELD_S32(0x49)
#define  oChainChompHitGate                      OBJECT_FIELD_S32(0x4A)

#define  oCheckerBoardPlatformUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oCheckerBoardPlatformUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oCheckerBoardPlatformUnk1AC OBJECT_FIELD_F32(0x49)

#define  oCheepCheepUnkF4  OBJECT_FIELD_F32(0x1B)
#define  oCheepCheepUnkF8  OBJECT_FIELD_F32(0x1C)
#define  oCheepCheepUnkFC  OBJECT_FIELD_F32(0x1D)
#define  oCheepCheepUnk104 OBJECT_FIELD_F32(0x1F)
#define  oCheepCheepUnk108 OBJECT_FIELD_F32(0x20)

#define  oChuckyaUnk88  OBJECT_FIELD_S32(0x00)
#define  oChuckyaUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oChuckyaUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oChuckyaUnk100 OBJECT_FIELD_S32(0x1E)

#define  oClamUnkF4 OBJECT_FIELD_S32(0x1B)

#define  oCloudCenterX              OBJECT_FIELD_F32(0x1B)
#define  oCloudCenterY              OBJECT_FIELD_F32(0x1C)
#define  oCloudBlowing              OBJECT_FIELD_S32(0x1D)
#define  oCloudGrowSpeed            OBJECT_FIELD_F32(0x1E)
#define  oCloudFwooshMovementRadius OBJECT_FIELD_S16(0x49, 0)

#define  oCoinCollectedFlags       OBJECT_FIELD_S32(0x1B)
#define  oCoinOnGround             OBJECT_FIELD_S32(0x1C)
#define  oCoinBaseVelY             OBJECT_FIELD_F32(0x22)
#ifndef VERSION_JP
#define  oCoinNumBounceSoundPlayed OBJECT_FIELD_S32(0x4A)
#endif

#define  oCollisionParticleUnkF4 OBJECT_FIELD_F32(0x1B)

#define  oControllablePlatformUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oControllablePlatformUnkFC  OBJECT_FIELD_F32(0x1D)
#define  oControllablePlatformUnk100 OBJECT_FIELD_S32(0x1E)

#define  oBreakableBoxSmallReleased            OBJECT_FIELD_S32(0x1B)
#define  oBreakableBoxSmallFramesSinceReleased OBJECT_FIELD_S32(0x1D)

#define  oJumpingBoxUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oJumpingBoxUnkF8 OBJECT_FIELD_S32(0x1C)

#define  oRRCruiserWingUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oRRCruiserWingUnkF8 OBJECT_FIELD_S32(0x1C)

#define  oDonutPlatformSpawnerSpawnedPlatforms OBJECT_FIELD_S32(0x1B)

#define  oDoorUnk88  OBJECT_FIELD_S32(0x00)
#define  oDoorUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oDoorUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oDoorUnk100 OBJECT_FIELD_S32(0x1E)

#define  oDorrieDistToHome         OBJECT_FIELD_F32(0x1B)
#define  oDorrieOffsetY            OBJECT_FIELD_F32(0x1C)
#define  oDorrieVelY               OBJECT_FIELD_F32(0x1D)
#define  oDorrieForwardDistToMario OBJECT_FIELD_F32(0x1E)
#define  oDorrieYawVel             OBJECT_FIELD_S32(0x1F)
#define  oDorrieLiftingMario       OBJECT_FIELD_S32(0x21)
#define  oDorrieGroundPounded      OBJECT_FIELD_S16(0x49, 0)
#define  oDorrieAngleToHome        OBJECT_FIELD_S16(0x49, 1)
#define  oDorrieNeckAngle          OBJECT_FIELD_S16(0x4A, 0)
#define  oDorrieHeadRaiseSpeed     OBJECT_FIELD_S16(0x4A, 1)

#define  oElevatorUnkF4  OBJECT_FIELD_F32(0x1B)
#define  oElevatorUnkF8  OBJECT_FIELD_F32(0x1C)
#define  oElevatorUnkFC  OBJECT_FIELD_F32(0x1D)
#define  oElevatorUnk100 OBJECT_FIELD_S32(0x1E)

#define  oExclamationBoxUnkF4 OBJECT_FIELD_F32(0x1B)
#define  oExclamationBoxUnkF8 OBJECT_FIELD_F32(0x1C)
#define  oExclamationBoxUnkFC OBJECT_FIELD_S32(0x1D)

#define  oEyerokBossNumHands   OBJECT_FIELD_S32(0x1C)
#define  oEyerokBossUnkFC      OBJECT_FIELD_S32(0x1D)
#define  oEyerokBossActiveHand OBJECT_FIELD_S32(0x1E)
#define  oEyerokBossUnk104     OBJECT_FIELD_S32(0x1F)
#define  oEyerokBossUnk108     OBJECT_FIELD_F32(0x20)
#define  oEyerokBossUnk10C     OBJECT_FIELD_F32(0x21)
#define  oEyerokBossUnk110     OBJECT_FIELD_F32(0x22)
#define  oEyerokBossUnk1AC     OBJECT_FIELD_S32(0x49)

#define  oEyerokHandWakeUpTimer OBJECT_FIELD_S32(0x1B)
#define  oEyerokReceivedAttack  OBJECT_FIELD_S32(0x1C)
#define  oEyerokHandUnkFC       OBJECT_FIELD_S32(0x1D)
#define  oEyerokHandUnk100      OBJECT_FIELD_S32(0x1E)

#define  oFallingPillarPitchAcceleration OBJECT_FIELD_F32(0x1B)

#define  oFireSpitterScaleVel OBJECT_FIELD_F32(0x1B)

#define  oBlueFishRandomVel   OBJECT_FIELD_F32(0x1B)
#define  oBlueFishRandomTime  OBJECT_FIELD_S32(0x1C)
#define  oBlueFishRandomAngle OBJECT_FIELD_F32(0x1E)

#define  oFishWaterLevel     OBJECT_FIELD_F32(0x1B)
#define  oFishGoalY          OBJECT_FIELD_F32(0x1C)
#define  oFishHeightOffset   OBJECT_FIELD_F32(0x1D)
#define  oFishYawVel         OBJECT_FIELD_S32(0x1E)
#define  oFishRoamDistance   OBJECT_FIELD_F32(0x1F)
#define  oFishGoalVel        OBJECT_FIELD_F32(0x20)
#define  oFishDepthDistance  OBJECT_FIELD_F32(0x21)
#define  oFishActiveDistance OBJECT_FIELD_F32(0x22)

#define  oFlameScale            OBJECT_FIELD_F32(0x1B)
#define  oFlameSpeedTimerOffset OBJECT_FIELD_S32(0x1C)
#define  oFlameUnusedRand       OBJECT_FIELD_F32(0x1D)
#define  oFlameBowser           OBJECT_FIELD_OBJ(0x1E)

#define  oBlueFlameNextScale OBJECT_FIELD_F32(0x1C)

#define  oSmallPiranhaFlameStartSpeed     OBJECT_FIELD_F32(0x1B)
#define  oSmallPiranhaFlameEndSpeed       OBJECT_FIELD_F32(0x1C)
#define  oSmallPiranhaFlameModel          OBJECT_FIELD_S32(0x1D)
#define  oSmallPiranhaFlameNextFlameTimer OBJECT_FIELD_S32(0x1E)
#define  oSmallPiranhaFlameSpeed          OBJECT_FIELD_F32(0x1F)

#define  oMovingFlameTimer OBJECT_FIELD_S32(0x1B)

#define  oFlameThowerFlameUnk110 OBJECT_FIELD_S32(0x22)

#define  oFlameThowerUnk110 OBJECT_FIELD_S32(0x22)

#define  oFloatingPlatformUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oFloatingPlatformUnkF8  OBJECT_FIELD_F32(0x1C)
#define  oFloatingPlatformUnkFC  OBJECT_FIELD_F32(0x1D)
#define  oFloatingPlatformUnk100 OBJECT_FIELD_S32(0x1E)

#define  oFloorSwitchPressAnimationUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oFloorSwitchPressAnimationUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oFloorSwitchPressAnimationUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oFloorSwitchPressAnimationUnk100 OBJECT_FIELD_S32(0x1E)

#define  oFlyGuyIdleTimer        OBJECT_FIELD_S32(0x1B)
#define  oFlyGuyOscTimer         OBJECT_FIELD_S32(0x1C)
#define  oFlyGuyUnusedJitter     OBJECT_FIELD_S32(0x1D)
#define  oFlyGuyLungeYDecel      OBJECT_FIELD_F32(0x1E)
#define  oFlyGuyLungeTargetPitch OBJECT_FIELD_S32(0x1F)
#define  oFlyGuyTargetRoll       OBJECT_FIELD_S32(0x20)
#define  oFlyGuyScaleVel         OBJECT_FIELD_F32(0x21)

#define  oGrandStarUnk108 OBJECT_FIELD_S32(0x20)

#define  oHorizontalGrindelTargetYaw  OBJECT_FIELD_S32(0x1B)
#define  oHorizontalGrindelDistToHome OBJECT_FIELD_F32(0x1C)
#define  oHorizontalGrindelOnGround   OBJECT_FIELD_S32(0x1D)

#define  oGoombaSize                OBJECT_FIELD_S32(0x1B)
#define  oGoombaScale               OBJECT_FIELD_F32(0x1C)
#define  oGoombaWalkTimer           OBJECT_FIELD_S32(0x1D)
#define  oGoombaTargetYaw           OBJECT_FIELD_S32(0x1E)
#define  oGoombaBlinkTimer          OBJECT_FIELD_S32(0x1F)
#define  oGoombaTurningAwayFromWall OBJECT_FIELD_S32(0x20)
#define  oGoombaRelativeSpeed       OBJECT_FIELD_F32(0x21)

#define  oHauntedChairUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oHauntedChairUnkF8  OBJECT_FIELD_F32(0x1C)
#define  oHauntedChairUnkFC  OBJECT_FIELD_F32(0x1D)
#define  oHauntedChairUnk100 OBJECT_FIELD_S32P(0x1E)
#define  oHauntedChairUnk104 OBJECT_FIELD_S32(0x1F)

#define  oHeaveHoUnk88 OBJECT_FIELD_S32(0x00)
#define  oHeaveHoUnkF4 OBJECT_FIELD_F32(0x1B)

#define  oHiddenObjectPurpleSwitch OBJECT_FIELD_OBJ(0x1B)

#define  oHootAvailability     OBJECT_FIELD_S32(0x1B)
#define  oHootMarioReleaseTime OBJECT_FIELD_S32(0x22)

#define  oHorizontalMovementUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oHorizontalMovementUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oHorizontalMovementUnk100 OBJECT_FIELD_F32(0x1E)
#define  oHorizontalMovementUnk104 OBJECT_FIELD_S32(0x1F)
#define  oHorizontalMovementUnk108 OBJECT_FIELD_F32(0x20)

#define  oKickableBoardF4 OBJECT_FIELD_S32(0x1B)
#define  oKickableBoardF8 OBJECT_FIELD_S32(0x1C)

#define  oKingBobombUnk88  OBJECT_FIELD_S32(0x00)
#define  oKingBobombUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oKingBobombUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oKingBobombUnk100 OBJECT_FIELD_S32(0x1E)
#define  oKingBobombUnk104 OBJECT_FIELD_S32(0x1F)
#define  oKingBobombUnk108 OBJECT_FIELD_S32(0x20)

#define  oKleptoDistanceToTarget      OBJECT_FIELD_F32(0x1B)
#define  oKleptoUnkF8                 OBJECT_FIELD_F32(0x1C)
#define  oKleptoUnkFC                 OBJECT_FIELD_F32(0x1D)
#define  oKleptoSpeed                 OBJECT_FIELD_F32(0x1E)
#define  oKleptoStartPosX             OBJECT_FIELD_F32(0x1F)
#define  oKleptoStartPosY             OBJECT_FIELD_F32(0x20)
#define  oKleptoStartPosZ             OBJECT_FIELD_F32(0x21)
#define  oKleptoTimeUntilTargetChange OBJECT_FIELD_S32(0x22)
#define  oKleptoTargetNumber          OBJECT_FIELD_S16(0x49, 0)
#define  oKleptoUnk1AE                OBJECT_FIELD_S16(0x49, 1)
#define  oKleptoUnk1B0                OBJECT_FIELD_S16(0x4A, 0)
#define  oKleptoYawToTarget           OBJECT_FIELD_S16(0x4A, 1)

#define  oKoopaAgility                     OBJECT_FIELD_F32(0x1B)
#define  oKoopaMovementType                OBJECT_FIELD_S32(0x1C)
#define  oKoopaTargetYaw                   OBJECT_FIELD_S32(0x1D)
#define  oKoopaUnshelledTimeUntilTurn      OBJECT_FIELD_S32(0x1E)
#define  oKoopaTurningAwayFromWall         OBJECT_FIELD_S32(0x1F)
#define  oKoopaDistanceToMario             OBJECT_FIELD_F32(0x20)
#define  oKoopaAngleToMario                OBJECT_FIELD_S32(0x21)
#define  oKoopaBlinkTimer                  OBJECT_FIELD_S32(0x22)
#define  oKoopaCountdown                   OBJECT_FIELD_S16(0x49, 0)
#define  oKoopaTheQuickRaceIndex           OBJECT_FIELD_S16(0x49, 1)
#define  oKoopaTheQuickInitTextboxCooldown OBJECT_FIELD_S16(0x4A, 0)

#define  oKoopaRaceEndpointRaceBegun     OBJECT_FIELD_S32(0x1B)
#define  oKoopaRaceEndpointKoopaFinished OBJECT_FIELD_S32(0x1C)
#define  oKoopaRaceEndpointRaceStatus    OBJECT_FIELD_S32(0x1D)
#define  oKoopaRaceEndpointDialog        OBJECT_FIELD_S32(0x1E)
#define  oKoopaRaceEndpointRaceEnded     OBJECT_FIELD_S32(0x1F)

#define  oKoopaShellFlameUnkF4 OBJECT_FIELD_F32(0x1B)
#define  oKoopaShellFlameScale OBJECT_FIELD_F32(0x1C)

#define  oCameraLakituBlinkTimer     OBJECT_FIELD_S32(0x1B)
#define  oCameraLakituSpeed          OBJECT_FIELD_F32(0x1C)
#define  oCameraLakituCircleRadius   OBJECT_FIELD_F32(0x1D)
#define  oCameraLakituFinishedDialog OBJECT_FIELD_S32(0x1E)
#ifndef VERSION_JP
#define  oCameraLakituUnk104         OBJECT_FIELD_S32(0x1F)
#endif
#define  oCameraLakituPitchVel       OBJECT_FIELD_S16(0x49, 0)
#define  oCameraLakituYawVel         OBJECT_FIELD_S16(0x49, 1)

#define  oEnemyLakituNumSpinies           OBJECT_FIELD_S32(0x1B)
#define  oEnemyLakituBlinkTimer           OBJECT_FIELD_S32(0x1C)
#define  oEnemyLakituSpinyCooldown        OBJECT_FIELD_S32(0x1D)
#define  oEnemyLakituFaceForwardCountdown OBJECT_FIELD_S32(0x1E)

#define  oIntroLakituSplineSegmentProgress OBJECT_FIELD_F32(0x1C)
#define  oIntroLakituSplineSegment         OBJECT_FIELD_F32(0x1D)
#define  oIntroLakituUnk100                OBJECT_FIELD_F32(0x1E)
#define  oIntroLakituUnk104                OBJECT_FIELD_F32(0x1F)
#define  oIntroLakituUnk108                OBJECT_FIELD_F32(0x20)
#define  oIntroLakituUnk10C                OBJECT_FIELD_F32(0x21)
#define  oIntroLakituUnk110                OBJECT_FIELD_F32(0x22)
#define  oIntroLakituCloud                 OBJECT_FIELD_OBJ(0x49)

#define  oMenuButtonState       OBJECT_FIELD_S32(0x1B)
#define  oMenuButtonTimer       OBJECT_FIELD_S32(0x1C)
#define  oMenuButtonOrigPosX    OBJECT_FIELD_F32(0x1D)
#define  oMenuButtonOrigPosY    OBJECT_FIELD_F32(0x1E)
#define  oMenuButtonOrigPosZ    OBJECT_FIELD_F32(0x1F)
#define  oMenuButtonScale       OBJECT_FIELD_F32(0x20)
#define  oMenuButtonActionPhase OBJECT_FIELD_S32(0x21)

#define  oMantaTargetPitch OBJECT_FIELD_S32(0x1B)
#define  oMantaTargetYaw   OBJECT_FIELD_S32(0x1C)

#define  oMerryGoRoundStopped         OBJECT_FIELD_S32(0x00)
#define  oMerryGoRoundMusicShouldPlay OBJECT_FIELD_S32(0x1C)
#define  oMerryGoRoundMarioIsOutside  OBJECT_FIELD_S32(0x1D)

#define  oMerryGoRoundBooManagerNumBoosKilled  OBJECT_FIELD_S32(0x00)
#define  oMerryGoRoundBooManagerNumBoosSpawned OBJECT_FIELD_S32(0x1D)

#define  oMipsStarStatus         OBJECT_FIELD_S32(0x1B)
#define  oMipsStartWaypointIndex OBJECT_FIELD_S32(0x1C)

#define  oMipsForwardVelocity    OBJECT_FIELD_F32(0x49)

#define  oMoneybagJumpState OBJECT_FIELD_S32(0x1B)

#define  oMontyMoleCurrentHole           OBJECT_FIELD_OBJ(0x1B)
#define  oMontyMoleHeightRelativeToFloor OBJECT_FIELD_F32(0x1C)

#define  oMontyMoleHoleCooldown OBJECT_FIELD_S32(0x1B)

#define  oMrBlizzardScale             OBJECT_FIELD_F32(0x1B)
#define  oMrBlizzardHeldObj           OBJECT_FIELD_OBJ(0x1C)
#define  oMrBlizzardGraphYVel         OBJECT_FIELD_F32(0x1D)
#define  oMrBlizzardTimer             OBJECT_FIELD_S32(0x1E)
#define  oMrBlizzardDizziness         OBJECT_FIELD_F32(0x1F)
#define  oMrBlizzardChangeInDizziness OBJECT_FIELD_F32(0x20)
#define  oMrBlizzardGraphYOffset      OBJECT_FIELD_F32(0x21)
#define  oMrBlizzardDistFromHome      OBJECT_FIELD_S32(0x22)
#define  oMrBlizzardTargetMoveYaw     OBJECT_FIELD_S32(0x49)

#define  oMrIUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oMrIUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oMrIUnk100 OBJECT_FIELD_S32(0x1E)
#define  oMrIUnk104 OBJECT_FIELD_S32(0x1F)
#define  oMrIUnk108 OBJECT_FIELD_S32(0x20)
#define  oMrIScale  OBJECT_FIELD_F32(0x21)
#define  oMrIUnk110 OBJECT_FIELD_S32(0x22)

#define  oRespawnerModelToRespawn    OBJECT_FIELD_S32(0x1B)
#define  oRespawnerMinSpawnDist      OBJECT_FIELD_F32(0x1C)
#define  oRespawnerBehaviorToRespawn OBJECT_FIELD_CVPTR(0x1D)

#define  oOpenableGrillUnk88        OBJECT_FIELD_S32(0x00)
#define  oOpenableGrillPurpleSwitch OBJECT_FIELD_OBJ(0x1B)

#define  oIntroPeachYawFromFocus OBJECT_FIELD_F32(0x20)
#define  oIntroPeachPitchFromFocus OBJECT_FIELD_F32(0x21)
#define  oIntroPeachDistToCamera OBJECT_FIELD_F32(0x22)

#define  oRacingPenguinInitTextCooldown       OBJECT_FIELD_S32(0x1B)

#define  oRacingPenguinWeightedNewTargetSpeed OBJECT_FIELD_F32(0x22)
#define  oRacingPenguinFinalTextbox           OBJECT_FIELD_S16(0x49, 0)
#define  oRacingPenguinMarioWon               OBJECT_FIELD_S16(0x49, 1)
#define  oRacingPenguinReachedBottom          OBJECT_FIELD_S16(0x4A, 0)
#define  oRacingPenguinMarioCheated           OBJECT_FIELD_S16(0x4A, 1)

#define  oSmallPenguinUnk88  OBJECT_FIELD_S32(0x00)
#define  oSmallPenguinUnk100 OBJECT_FIELD_S32(0x1E)
#define  oSmallPenguinUnk104 OBJECT_FIELD_F32(0x1F)
#define  oSmallPenguinUnk108 OBJECT_FIELD_F32(0x20)
#define  oSmallPenguinUnk110 OBJECT_FIELD_S32(0x22)

#define  oSLWalkingPenguinWindCollisionXPos OBJECT_FIELD_F32(0x1E)
#define  oSLWalkingPenguinWindCollisionZPos OBJECT_FIELD_F32(0x1F)
#define  oSLWalkingPenguinCurStep           OBJECT_FIELD_S32(0x21)
#define  oSLWalkingPenguinCurStepTimer      OBJECT_FIELD_S32(0x22)

#define  oPiranhaPlantSleepMusicState OBJECT_FIELD_S32(0x1B)
#define  oPiranhaPlantScale           OBJECT_FIELD_F32(0x1C)

#define  oFirePiranhaPlantNeutralScale   OBJECT_FIELD_F32(0x1B)
#define  oFirePiranhaPlantScale          OBJECT_FIELD_F32(0x1C)
#define  oFirePiranhaPlantActive         OBJECT_FIELD_S32(0x1D)
#define  oFirePiranhaPlantDeathSpinTimer OBJECT_FIELD_S32(0x1E)
#define  oFirePiranhaPlantDeathSpinVel   OBJECT_FIELD_F32(0x1F)

#define  oPitouneUnkF4 OBJECT_FIELD_F32(0x1B)
#define  oPitouneUnkF8 OBJECT_FIELD_F32(0x1C)
#define  oPitouneUnkFC OBJECT_FIELD_F32(0x1D)

#define  oBitFSPlatformTimer   OBJECT_FIELD_S32(0x1B)

#define  oBitSPlatformBowser   OBJECT_FIELD_OBJ(0x1C)
#define  oBitSPlatformTimer    OBJECT_FIELD_S32(0x1D)

#define  oPlatformUnk10C OBJECT_FIELD_F32(0x21)
#define  oPlatformUnk110 OBJECT_FIELD_F32(0x22)

#define  oPlatformOnTrackBaseBallIndex          OBJECT_FIELD_S32(0x00)
#define  oPlatformOnTrackDistMovedSinceLastBall OBJECT_FIELD_F32(0x1B)
#define  oPlatformOnTrackSkiLiftRollVel         OBJECT_FIELD_F32(0x1C)
#define  oPlatformOnTrackStartWaypoint          OBJECT_FIELD_WAYPOINT(0x1D)
#define  oPlatformOnTrackPrevWaypoint           OBJECT_FIELD_WAYPOINT(0x1E)
#define  oPlatformOnTrackPrevWaypointFlags      OBJECT_FIELD_S32(0x1F)
#define  oPlatformOnTrackPitch                  OBJECT_FIELD_S32(0x20)
#define  oPlatformOnTrackYaw                    OBJECT_FIELD_S32(0x21)
#define  oPlatformOnTrackOffsetY                OBJECT_FIELD_F32(0x22)
#define  oPlatformOnTrackIsNotSkiLift           OBJECT_FIELD_S16(0x49, 0)
#define  oPlatformOnTrackIsNotHMC               OBJECT_FIELD_S16(0x49, 1)
#define  oPlatformOnTrackType                   OBJECT_FIELD_S16(0x4A, 0)
#define  oPlatformOnTrackWasStoodOn             OBJECT_FIELD_S16(0x4A, 1)

#define  oPlatformSpawnerUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oPlatformSpawnerUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oPlatformSpawnerUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oPlatformSpawnerUnk100 OBJECT_FIELD_F32(0x1E)
#define  oPlatformSpawnerUnk104 OBJECT_FIELD_F32(0x1F)
#define  oPlatformSpawnerUnk108 OBJECT_FIELD_F32(0x20)

#define  oPokeyAliveBodyPartFlags  OBJECT_FIELD_U32(0x1B)
#define  oPokeyNumAliveBodyParts   OBJECT_FIELD_S32(0x1C)
#define  oPokeyBottomBodyPartSize  OBJECT_FIELD_F32(0x1D)
#define  oPokeyHeadWasKilled       OBJECT_FIELD_S32(0x1E)
#define  oPokeyTargetYaw           OBJECT_FIELD_S32(0x1F)
#define  oPokeyChangeTargetTimer   OBJECT_FIELD_S32(0x20)
#define  oPokeyTurningAwayFromWall OBJECT_FIELD_S32(0x21)

#define  oPokeyBodyPartDeathDelayAfterHeadKilled OBJECT_FIELD_S32(0x1C)
#define  oPokeyBodyPartBlinkTimer                OBJECT_FIELD_S32(0x22)

#define  oDDDPoleVel       OBJECT_FIELD_F32(0x1B)
#define  oDDDPoleMaxOffset OBJECT_FIELD_F32(0x1C)
#define  oDDDPoleOffset    OBJECT_FIELD_F32(0x1D)

#define  oPyramidTopPillarsTouched OBJECT_FIELD_S32(0x1B)

#define  oPyramidTopFragmentsScale OBJECT_FIELD_F32(0x1B)

#define  oRollingLogUnkF4 OBJECT_FIELD_F32(0x1B)

#define  oLLLRotatingHexFlameUnkF4 OBJECT_FIELD_F32(0x1B)
#define  oLLLRotatingHexFlameUnkF8 OBJECT_FIELD_F32(0x1C)
#define  oLLLRotatingHexFlameUnkFC OBJECT_FIELD_F32(0x1D)

#define  oScuttlebugUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oScuttlebugUnkF8 OBJECT_FIELD_S32(0x1C)
#define  oScuttlebugUnkFC OBJECT_FIELD_S32(0x1D)

#define  oScuttlebugSpawnerUnk88 OBJECT_FIELD_S32(0x00)
#define  oScuttlebugSpawnerUnkF4 OBJECT_FIELD_S32(0x1B)

#define  oSeesawPlatformPitchVel OBJECT_FIELD_F32(0x1B)

#define  oShipPart3UnkF4 OBJECT_FIELD_S32(0x1B)
#define  oShipPart3UnkF8 OBJECT_FIELD_S32(0x1C)

#define  oSinkWhenSteppedOnUnk104 OBJECT_FIELD_S32(0x1F)
#define  oSinkWhenSteppedOnUnk108 OBJECT_FIELD_F32(0x20)

#define  oSkeeterTargetAngle         OBJECT_FIELD_S32(0x1B)
#define  oSkeeterTurningAwayFromWall OBJECT_FIELD_S32(0x1C)
#define  oSkeeterUnkFC               OBJECT_FIELD_F32(0x1D)
#define  oSkeeterWaitTime            OBJECT_FIELD_S32(0x1E)
#define  oSkeeterUnk1AC              OBJECT_FIELD_S16(0x49, 0)

#define  oJRBSlidingBoxUnkF4 OBJECT_FIELD_OBJ(0x1B)
#define  oJRBSlidingBoxUnkF8 OBJECT_FIELD_S32(0x1C)
#define  oJRBSlidingBoxUnkFC OBJECT_FIELD_F32(0x1D)

#define  oWFSlidBrickPtfmMovVel OBJECT_FIELD_F32(0x1B)

#define  oSmokeTimer OBJECT_FIELD_S32(0x1B)

#define  oSnowmansBottomScale  OBJECT_FIELD_F32(0x1B)
#define  oSnowmansBottomUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oSnowmansBottomUnk1AC OBJECT_FIELD_S32(0x49)

#define  oSnowmansHeadDialogActive OBJECT_FIELD_S32(0x1B)

#define  oSLSnowmanWindOriginalYaw OBJECT_FIELD_S32(0x1B)

#define  oSnufitRecoil          OBJECT_FIELD_S32(0x1B)
#define  oSnufitScale           OBJECT_FIELD_F32(0x1C)
#define  oSnufitCircularPeriod  OBJECT_FIELD_S32(0x1E)
#define  oSnufitBodyScalePeriod OBJECT_FIELD_S32(0x1F)
#define  oSnufitBodyBaseScale   OBJECT_FIELD_S32(0x20)
#define  oSnufitBullets         OBJECT_FIELD_S32(0x21)
#define  oSnufitXOffset         OBJECT_FIELD_S16(0x49, 0)
#define  oSnufitYOffset         OBJECT_FIELD_S16(0x49, 1)
#define  oSnufitZOffset         OBJECT_FIELD_S16(0x4A, 0)
#define  oSnufitBodyScale       OBJECT_FIELD_S16(0x4A, 1)

#define  oSpindelUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oSpindelUnkF8 OBJECT_FIELD_S32(0x1C)

#define  oSpinningHeartTotalSpin   OBJECT_FIELD_S32(0x1B)
#define  oSpinningHeartPlayedSound OBJECT_FIELD_S32(0x1C)

#define  oSpinyTimeUntilTurn       OBJECT_FIELD_S32(0x1B)
#define  oSpinyTargetYaw           OBJECT_FIELD_S32(0x1C)
#define  oSpinyTurningAwayFromWall OBJECT_FIELD_S32(0x1E)

#define  oSoundEffectUnkF4 OBJECT_FIELD_S32(0x1B)

#define  oStarSpawnDisFromHome OBJECT_FIELD_F32(0x1B)
#define  oStarSpawnUnkFC       OBJECT_FIELD_F32(0x1D)

#define  oHiddenStarTriggerCounter OBJECT_FIELD_S32(0x1B)

#define  oSparkleSpawnUnk1B0 OBJECT_FIELD_S32(0x4A)

#define  oUnlockDoorStarState  OBJECT_FIELD_U32(0x20)
#define  oUnlockDoorStarTimer  OBJECT_FIELD_S32(0x21)
#define  oUnlockDoorStarYawVel OBJECT_FIELD_S32(0x22)

#define  oCelebStarUnkF4              OBJECT_FIELD_S32(0x1B)
#define  oCelebStarDiameterOfRotation OBJECT_FIELD_S32(0x20)

#define  oStarSelectorType  OBJECT_FIELD_S32(0x1B)
#define  oStarSelectorTimer OBJECT_FIELD_S32(0x1C)
#define  oStarSelectorSize  OBJECT_FIELD_F32(0x20)

#define  oSushiSharkUnkF4 OBJECT_FIELD_S32(0x1B)

#define  oSwingPlatformAngle OBJECT_FIELD_F32(0x1B)
#define  oSwingPlatformSpeed OBJECT_FIELD_F32(0x1C)

#define  oSwoopBonkCountdown OBJECT_FIELD_S32(0x1B)
#define  oSwoopTargetPitch   OBJECT_FIELD_S32(0x1C)
#define  oSwoopTargetYaw     OBJECT_FIELD_S32(0x1D)

#define  oGrindelThwompRandomTimer OBJECT_FIELD_S32(0x1B)

#define  oTiltingPyramidNormalX         OBJECT_FIELD_F32(0x1B)
#define  oTiltingPyramidNormalY         OBJECT_FIELD_F32(0x1C)
#define  oTiltingPyramidNormalZ         OBJECT_FIELD_F32(0x1D)
#define  oTiltingPyramidMarioOnPlatform OBJECT_FIELD_S32(0x21)

#define  oToadMessageDialogID       OBJECT_FIELD_U32(0x20)
#define  oToadMessageRecentlyTalked OBJECT_FIELD_S32(0x21)
#define  oToadMessageState          OBJECT_FIELD_S32(0x22)

#define  oToxBoxActionTable OBJECT_FIELD_VPTR(0x49)
#define  oToxBoxActionStep  OBJECT_FIELD_S32(0x4A)

#define  oTTCRotatingSolidNumTurns      OBJECT_FIELD_S32(0x1B)
#define  oTTCRotatingSolidNumSides      OBJECT_FIELD_S32(0x1C)
#define  oTTCRotatingSolidRotationDelay OBJECT_FIELD_S32(0x1D)
#define  oTTCRotatingSolidVelY          OBJECT_FIELD_F32(0x1E)
#define  oTTCRotatingSolidSoundTimer    OBJECT_FIELD_S32(0x1F)

#define  oTTCPendulumAccelDir   OBJECT_FIELD_F32(0x1B)
#define  oTTCPendulumAngle      OBJECT_FIELD_F32(0x1C)
#define  oTTCPendulumAngleVel   OBJECT_FIELD_F32(0x1D)
#define  oTTCPendulumAngleAccel OBJECT_FIELD_F32(0x1E)
#define  oTTCPendulumDelay      OBJECT_FIELD_S32(0x1F)
#define  oTTCPendulumSoundTimer OBJECT_FIELD_S32(0x20)

#define  oTTCTreadmillBigSurface      OBJECT_FIELD_S16P(0x1B)
#define  oTTCTreadmillSmallSurface    OBJECT_FIELD_S16P(0x1C)
#define  oTTCTreadmillSpeed           OBJECT_FIELD_F32(0x1D)
#define  oTTCTreadmillTargetSpeed     OBJECT_FIELD_F32(0x1E)
#define  oTTCTreadmillTimeUntilSwitch OBJECT_FIELD_S32(0x1F)

#define  oTTCMovingBarDelay        OBJECT_FIELD_S32(0x1B)
#define  oTTCMovingBarStoppedTimer OBJECT_FIELD_S32(0x1C)
#define  oTTCMovingBarOffset       OBJECT_FIELD_F32(0x1D)
#define  oTTCMovingBarSpeed        OBJECT_FIELD_F32(0x1E)
#define  oTTCMovingBarStartOffset  OBJECT_FIELD_F32(0x1F)

#define  oTTCCogDir       OBJECT_FIELD_F32(0x1B)
#define  oTTCCogSpeed     OBJECT_FIELD_F32(0x1C)
#define  oTTCCogTargetVel OBJECT_FIELD_F32(0x1D)

#define  oTTCPitBlockPeakY    OBJECT_FIELD_F32(0x1B)
#define  oTTCPitBlockDir      OBJECT_FIELD_S32(0x1C)
#define  oTTCPitBlockWaitTime OBJECT_FIELD_S32(0x1D)

#define  oTTCElevatorDir      OBJECT_FIELD_F32(0x1B)
#define  oTTCElevatorPeakY    OBJECT_FIELD_F32(0x1C)
#define  oTTCElevatorMoveTime OBJECT_FIELD_S32(0x1D)

#define  oTTC2DRotatorMinTimeUntilNextTurn OBJECT_FIELD_S32(0x1B)
#define  oTTC2DRotatorTargetYaw            OBJECT_FIELD_S32(0x1C)
#define  oTTC2DRotatorIncrement            OBJECT_FIELD_S32(0x1D)
#define  oTTC2DRotatorRandomDirTimer       OBJECT_FIELD_S32(0x1F)
#define  oTTC2DRotatorSpeed                OBJECT_FIELD_S32(0x20)

#define  oTTCSpinnerDir     OBJECT_FIELD_S32(0x1B)
#define  oTTCChangeDirTimer OBJECT_FIELD_S32(0x1C)

#define  oBetaTrampolineMarioOnTrampoline OBJECT_FIELD_S32(0x22)

#define  oTreasureChestUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oTreasureChestUnkF8 OBJECT_FIELD_S32(0x1C)
#define  oTreasureChestUnkFC OBJECT_FIELD_S32(0x1D)

#define  oTreeSnowOrLeafUnkF4 OBJECT_FIELD_S32(0x1B)
#define  oTreeSnowOrLeafUnkF8 OBJECT_FIELD_S32(0x1C)
#define  oTreeSnowOrLeafUnkFC OBJECT_FIELD_S32(0x1D)

#define  oTumblingBridgeUnkF4 OBJECT_FIELD_S32(0x1B)

#define  oTweesterScaleTimer OBJECT_FIELD_S32(0x1B)
#define  oTweesterUnused     OBJECT_FIELD_S32(0x1C)

#define  oUkikiTauntCounter   OBJECT_FIELD_S16(0x1B, 0)
#define  oUkikiTauntsToBeDone OBJECT_FIELD_S16(0x1B, 1)

#define  oUkikiChaseFleeRange OBJECT_FIELD_F32(0x22)
#define  oUkikiTextState      OBJECT_FIELD_S16(0x49, 0)
#define  oUkikiTextboxTimer   OBJECT_FIELD_S16(0x49, 1)
#define  oUkikiCageSpinTimer  OBJECT_FIELD_S16(0x4A, 0)
#define  oUkikiHasCap         OBJECT_FIELD_S16(0x4A, 1)

#define  oUkikiCageNextAction OBJECT_FIELD_S32(0x00)

#define  oUnagiUnkF4  OBJECT_FIELD_F32(0x1B)
#define  oUnagiUnkF8  OBJECT_FIELD_F32(0x1C)

#define  oUnagiUnk110 OBJECT_FIELD_F32(0x22)
#define  oUnagiUnk1AC OBJECT_FIELD_F32(0x49)
#define  oUnagiUnk1B0 OBJECT_FIELD_S16(0x4A, 0)
#define  oUnagiUnk1B2 OBJECT_FIELD_S16(0x4A, 1)

#define  oWaterBombVerticalStretch OBJECT_FIELD_F32(0x1C)
#define  oWaterBombStretchSpeed    OBJECT_FIELD_F32(0x1D)
#define  oWaterBombOnGround        OBJECT_FIELD_S32(0x1E)
#define  oWaterBombNumBounces      OBJECT_FIELD_F32(0x1F)

#define  oWaterBombSpawnerBombActive  OBJECT_FIELD_S32(0x1B)
#define  oWaterBombSpawnerTimeToSpawn OBJECT_FIELD_S32(0x1C)

#define  oWaterCannonUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oWaterCannonUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oWaterCannonUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oWaterCannonUnk100 OBJECT_FIELD_S32(0x1E)

#define  oCannonBarrelBubblesUnkF4 OBJECT_FIELD_F32(0x1B)

#define  oWaterLevelPillarDrained OBJECT_FIELD_S32(0x1C)

#define  oWaterLevelTriggerUnkF4            OBJECT_FIELD_S32(0x1B)
#define  oWaterLevelTriggerTargetWaterLevel OBJECT_FIELD_S32(0x1C)

#define  oWaterObjUnkF4  OBJECT_FIELD_S32(0x1B)
#define  oWaterObjUnkF8  OBJECT_FIELD_S32(0x1C)
#define  oWaterObjUnkFC  OBJECT_FIELD_S32(0x1D)
#define  oWaterObjUnk100 OBJECT_FIELD_S32(0x1E)

#define  oWaterRingScalePhaseX      OBJECT_FIELD_S32(0x1B)
#define  oWaterRingScalePhaseY      OBJECT_FIELD_S32(0x1C)
#define  oWaterRingScalePhaseZ      OBJECT_FIELD_S32(0x1D)
#define  oWaterRingNormalX          OBJECT_FIELD_F32(0x1E)
#define  oWaterRingNormalY          OBJECT_FIELD_F32(0x1F)
#define  oWaterRingNormalZ          OBJECT_FIELD_F32(0x20)
#define  oWaterRingMarioDistInFront OBJECT_FIELD_F32(0x21)
#define  oWaterRingIndex            OBJECT_FIELD_S32(0x22)
#define  oWaterRingAvgScale         OBJECT_FIELD_F32(0x49)

#define  oWaterRingSpawnerRingsCollected OBJECT_FIELD_S32(0x49)

#define  oWaterRingMgrNextRingIndex     OBJECT_FIELD_S32(0x1B)
#define  oWaterRingMgrLastRingCollected OBJECT_FIELD_S32(0x1C)

#define  oWaveTrailSize OBJECT_FIELD_F32(0x1C)

#define  oWhirlpoolInitFacePitch OBJECT_FIELD_S32(0x1B)
#define  oWhirlpoolInitFaceRoll  OBJECT_FIELD_S32(0x1C)

#define  oWhitePuffUnkF4 OBJECT_FIELD_F32(0x1B)
#define  oWhitePuffUnkF8 OBJECT_FIELD_S32(0x1C)
#define  oWhitePuffUnkFC OBJECT_FIELD_S32(0x1D)

#define  oStrongWindParticlePenguinObj OBJECT_FIELD_OBJ(0x1B)

#define  oWhompShakeVal OBJECT_FIELD_S32(0x1C)

#define  oWigglerFallThroughFloorsHeight OBJECT_FIELD_F32(0x1B)
#define  oWigglerSegments                OBJECT_FIELD_CHAIN_SEGMENT(0x1C)
#define  oWigglerWalkAnimSpeed           OBJECT_FIELD_F32(0x1D)
#define  oWigglerSquishSpeed             OBJECT_FIELD_F32(0x1F)
#define  oWigglerTimeUntilRandomTurn     OBJECT_FIELD_S32(0x20)
#define  oWigglerTargetYaw               OBJECT_FIELD_S32(0x21)
#define  oWigglerWalkAwayFromWallTimer   OBJECT_FIELD_S32(0x22)
#define  oWigglerUnused                  OBJECT_FIELD_S16(0x49, 0)
#define  oWigglerTextStatus              OBJECT_FIELD_S16(0x49, 1)

#define  oLLLWoodPieceOscillationTimer OBJECT_FIELD_S32(0x1B)

#define  oWoodenPostTotalMarioAngle  OBJECT_FIELD_S32(0x1B)
#define  oWoodenPostPrevAngleToMario OBJECT_FIELD_S32(0x1C)
#define  oWoodenPostSpeedY           OBJECT_FIELD_F32(0x1D)
#define  oWoodenPostMarioPounding    OBJECT_FIELD_S32(0x1E)
#define  oWoodenPostOffsetY          OBJECT_FIELD_F32(0x1F)

#define  oYoshiBlinkTimer OBJECT_FIELD_S32(0x1B)
#define  oYoshiChosenHome OBJECT_FIELD_S32(0x1D)
#define  oYoshiTargetYaw  OBJECT_FIELD_S32(0x1E)

#endif
