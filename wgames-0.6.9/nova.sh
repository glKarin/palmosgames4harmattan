#!/bin/sh

HACK_DIR=`pwd`/hacks/ldhack

#env
#ldd "$2"
echo $LD_PRELOAD

#/opt/preenv/wrapper.sh "com.gameloft.app.nova" "/home/user/Games/N.O.V.A/nova"

#./wrapper-wgames.sh "com.gameloft.app.nova" "/home/user/Games/N.O.V.A/nova"
#./wrapper-wgames.sh "com.glu.app.ghero5" "/home/user/Games/Guitar Hero 5 Mobile/ghero5.exe"
#./wrapper-wgames.sh "com.glu.app.deer3d" "/home/user/Games/Deer Hunter/DH3D.exe"
#/usr/bin/env  LD_HACK_FROM="/media/internal/" LD_HACK_TO="/home/user/Games/Asphalt5/"  LD_POWERVR_DIR="/home/user/Games/Asphalt5/" LD_PRELOAD=libldhack.so ./wrapper-wgames.sh "com.gameloft.app.asphalt5" "/home/user/Games/Asphalt5/Asphalt5"
#/usr/bin/env  LD_POWERVR_DIR="/home/user/Games/Shrek Kart/" LD_PRELOAD=$HACK_DIR/libldhack.so ./wrapper-wgames.sh "com.gameloft.app.shrekkarting" "/home/user/Games/Shrek Kart/ShrekKarting"
#./wrapper-wgames.sh "com.gameloft.app.heroofsparta" "/home/user/Games/HeroOfSparta/HeroOfSparta"
#./wrapper-wgames.sh "com.gameloft.app.letsgolf" "/home/user/Games/Let's Golf/letsgolf"
#./wrapper-wgames.sh "com.polarbit.rthunder2" "/home/user/Games/Raging Thunder 2/rthunder2"
./wrapper-wgames.sh "com.gameloft.app.brainchallenge" "/home/user/Games/Brain Challenge/brainchallenge"
#./wrapper-wgames.sh "com.palmdts.app.speedforge" "/home/user/Games/Speed Forge Extreme/speedforge"
#/usr/bin/env  LD_HACK_FROM="/media/cryptofs/apps/" LD_HACK_TO="/home/user/MyDocs/Games/Hawx/" LD_PRELOAD=$HACK_DIR/libldhack.so "./wrapper-wgames.sh" "com.gameloft.app.hawx" "/home/user/Games/Hawx/hawx"
