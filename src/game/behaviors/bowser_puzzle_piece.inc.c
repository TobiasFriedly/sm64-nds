
static s8 sPieceActions01[] = { 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, -1 };
static s8 sPieceActions02[] = { 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 2, -1 };
static s8 sPieceActions05[] = { 2, 2, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 2, 2, -1 };
static s8 sPieceActions06[] = { 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, -1 };
static s8 sPieceActions10[] = { 2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 6, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions09[] = { 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions13[] = { 2, 2, 2, 2, 2, 2, 2, 5, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 6, 2, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions12[] = { 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2,
                                2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions08[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 2, 2, 2,
                                2, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions07[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 2, 2,
                                2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions03[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 2,
                                5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions04[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4,
                                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions11[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1 };
static s8 sPieceActions14[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1 };

struct BowserPuzzlePiece {
    u8 model;
    s8 xOffset;
    s8 zOffset;
    s8 initialAction;
    s8 *actionList;
};

static struct BowserPuzzlePiece sBowserPuzzlePieces[] = {
    { MODEL_LLL_BOWSER_PIECE_1, -5, -15, 1, sPieceActions01 },
    { MODEL_LLL_BOWSER_PIECE_2, 5, -15, 0, sPieceActions02 },
    { MODEL_LLL_BOWSER_PIECE_3, -15, -5, 0, sPieceActions03 },
    { MODEL_LLL_BOWSER_PIECE_4, -5, -5, 0, sPieceActions04 },
    { MODEL_LLL_BOWSER_PIECE_5, 5, -5, 0, sPieceActions05 },
    { MODEL_LLL_BOWSER_PIECE_6, 15, -5, 0, sPieceActions06 },
    { MODEL_LLL_BOWSER_PIECE_7, -15, 5, 0, sPieceActions07 },
    { MODEL_LLL_BOWSER_PIECE_8, -5, 5, 0, sPieceActions08 },
    { MODEL_LLL_BOWSER_PIECE_9, 5, 5, 0, sPieceActions09 },
    { MODEL_LLL_BOWSER_PIECE_10, 15, 5, 0, sPieceActions10 },
    { MODEL_LLL_BOWSER_PIECE_11, -15, 15, 0, sPieceActions11 },
    { MODEL_LLL_BOWSER_PIECE_12, -5, 15, 0, sPieceActions12 },
    { MODEL_LLL_BOWSER_PIECE_13, 5, 15, 0, sPieceActions13 },
    { MODEL_LLL_BOWSER_PIECE_14, 15, 15, 0, sPieceActions14 }
};

void bhv_lll_bowser_puzzle_spawn_piece(s16 model, const BehaviorScript *behavior,
                                       f32 xOffset, f32 zOffset,
                                       s8 initialAction, s8 *actionList) {
    struct Object *puzzlePiece = spawn_object(o, model, behavior);
    puzzlePiece->oPosX += xOffset;
    puzzlePiece->oPosY += 50.0f;
    puzzlePiece->oPosZ += zOffset;
    puzzlePiece->oAction = initialAction;
    puzzlePiece->oBowserPuzzlePieceActionList = actionList;
    puzzlePiece->oBowserPuzzlePieceNextAction = actionList;
}

void bhv_lll_bowser_puzzle_spawn_pieces(f32 pieceWidth) {
    s32 i;

    for (i = 0; i < 14; i++) {
        bhv_lll_bowser_puzzle_spawn_piece(sBowserPuzzlePieces[i].model, bhvLLLBowserPuzzlePiece,
                                          sBowserPuzzlePieces[i].xOffset * pieceWidth / 10.0f,
                                          sBowserPuzzlePieces[i].zOffset * pieceWidth / 10.0f,
                                          sBowserPuzzlePieces[i].initialAction,
                                          sBowserPuzzlePieces[i].actionList);
    }

    o->oAction++;
}

void bhv_lll_bowser_puzzle_loop(void) {
    s32 i;

    switch (o->oAction) {
        case BOWSER_PUZZLE_ACT_SPAWN_PIECES:
            bhv_lll_bowser_puzzle_spawn_pieces(480.0f);
            break;

        case BOWSER_PUZZLE_ACT_WAIT_FOR_COMPLETE:

            if (o->oBowserPuzzleCompletionFlags == 3 && o->oDistanceToMario < 1000.0f) {

                for (i = 0; i < 5; i++) {
                    UNUSED struct Object *coin =
                        spawn_object(o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);
                }

                o->oBowserPuzzleCompletionFlags = 0;

                o->oAction++;
            }
            break;

        case BOWSER_PUZZLE_ACT_DONE:
            break;
    }
}

void bhv_lll_bowser_puzzle_piece_action_0(void) {
}

void bhv_lll_bowser_puzzle_piece_action_1(void) {
    o->oPosY += 50.0f;
    o->oAction = 3;
}

void bhv_lll_bowser_puzzle_piece_update(void) {
    s8 *nextAction = o->oBowserPuzzlePieceNextAction;

    if (gMarioObject->platform == o) {
        o->parentObj->oBowserPuzzleCompletionFlags = 1;
    }

    if (o->oBowserPuzzlePieceContinuePerformingAction == 0) {

        cur_obj_change_action(*nextAction);

        nextAction++;
        o->oBowserPuzzlePieceNextAction = nextAction;

        if (*nextAction == -1) {

            o->parentObj->oBowserPuzzleCompletionFlags |= 2;

            o->oBowserPuzzlePieceNextAction = o->oBowserPuzzlePieceActionList;
        }

        o->oBowserPuzzlePieceContinuePerformingAction = 1;
    }
}

void bhv_lll_bowser_puzzle_piece_move(f32 xOffset, f32 zOffset, s32 duration, UNUSED s32 unused) {

    if (o->oTimer < 20) {
        if (o->oTimer % 2) {
            o->oBowserPuzzlePieceOffsetY = 0.0f;
        } else {
            o->oBowserPuzzlePieceOffsetY = -6.0f;
        }
    } else {

        if (o->oTimer == 20) {
            cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_PUZZLE_PIECE_MOVE);
        }

        if (o->oTimer < duration + 20) {
            o->oBowserPuzzlePieceOffsetX += xOffset;
            o->oBowserPuzzlePieceOffsetZ += zOffset;
        } else {

            o->oAction = 2;

            o->oBowserPuzzlePieceContinuePerformingAction = 0;
        }
    }
}

void bhv_lll_bowser_puzzle_piece_idle(void) {
    UNUSED s32 unused;

    if (o->oTimer < 24) {
        unused = 0;
    } else {

        o->oBowserPuzzlePieceContinuePerformingAction = 0;
    }
}

void bhv_lll_bowser_puzzle_piece_move_left(void) {
    bhv_lll_bowser_puzzle_piece_move(-120.0f, 0.0f, 4, 4);
}

void bhv_lll_bowser_puzzle_piece_move_right(void) {
    bhv_lll_bowser_puzzle_piece_move(120.0f, 0.0f, 4, 5);
}

void bhv_lll_bowser_puzzle_piece_move_up(void) {
    bhv_lll_bowser_puzzle_piece_move(0.0f, -120.0f, 4, 6);
}

void bhv_lll_bowser_puzzle_piece_move_down(void) {
    bhv_lll_bowser_puzzle_piece_move(0.0f, 120.0f, 4, 3);
}

void (*sBowserPuzzlePieceActions[])(void) = {
    bhv_lll_bowser_puzzle_piece_action_0,
    bhv_lll_bowser_puzzle_piece_action_1,
    bhv_lll_bowser_puzzle_piece_idle,
    bhv_lll_bowser_puzzle_piece_move_left,
    bhv_lll_bowser_puzzle_piece_move_right,
    bhv_lll_bowser_puzzle_piece_move_up,
    bhv_lll_bowser_puzzle_piece_move_down,
};

void bhv_lll_bowser_puzzle_piece_loop(void) {
    bhv_lll_bowser_puzzle_piece_update();

    cur_obj_call_action_function(sBowserPuzzlePieceActions);

    o->oPosX = o->oBowserPuzzlePieceOffsetX + o->oHomeX;
    o->oPosY = o->oBowserPuzzlePieceOffsetY + o->oHomeY;
    o->oPosZ = o->oBowserPuzzlePieceOffsetZ + o->oHomeZ;
}
