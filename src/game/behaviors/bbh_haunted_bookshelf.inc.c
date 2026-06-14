
void bhv_haunted_bookshelf_loop(void) {

    o->oDistanceToMario = dist_between_objects(o, gMarioObject);

    o->oFaceAngleYaw = 0;

    switch (o->oAction) {
        case HAUNTED_BOOKSHELF_ACT_IDLE:

            if (o->oTimer == 0) {
            }

            if (o->oHauntedBookshelfShouldOpen != FALSE) {
                o->oAction++;
            }

            break;

        case HAUNTED_BOOKSHELF_ACT_RECEDE:

            o->oPosX += 5.0f;
            cur_obj_play_sound_1(SOUND_ENV_ELEVATOR4_2);

            if (o->oTimer > 101) {
                obj_mark_for_deletion(o);
            }

            break;

        default:
            break;
    }
}
