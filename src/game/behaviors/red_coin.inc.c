
static struct ObjectHitbox sRedCoinHitbox = {
     INTERACT_COIN,
     0,
     2,
     0,
     0,
     100,
     64,
     0,
     0,
};

void bhv_red_coin_init(void) {

    struct Surface *dummyFloor;
    UNUSED f32 floorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &dummyFloor);

    struct Object *hiddenRedCoinStar;

    if ((hiddenRedCoinStar =
             cur_obj_nearest_object_with_behavior(bhvHiddenRedCoinStar)) != NULL) {
        o->parentObj = hiddenRedCoinStar;
    } else if ((hiddenRedCoinStar =
             cur_obj_nearest_object_with_behavior(bhvBowserCourseRedCoinStar)) != NULL) {
        o->parentObj = hiddenRedCoinStar;
    } else {
        o->parentObj = NULL;
    }

    obj_set_hitbox(o, &sRedCoinHitbox);
}

void bhv_red_coin_loop(void) {

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {

        if (o->parentObj != NULL) {

            o->parentObj->oHiddenStarTriggerCounter++;

#ifdef VERSION_JP
            create_sound_spawner(SOUND_GENERAL_RED_COIN);
#endif

            if (o->parentObj->oHiddenStarTriggerCounter != 8) {
                spawn_orange_number(o->parentObj->oHiddenStarTriggerCounter, 0, 0, 0);
            }

#ifndef VERSION_JP
            play_sound(SOUND_MENU_COLLECT_RED_COIN
                       + (((u8) o->parentObj->oHiddenStarTriggerCounter - 1) << 16),
                       gGlobalSoundSource);
#endif
        }

        coin_collected();

        o->oInteractStatus = 0;
    }
}
