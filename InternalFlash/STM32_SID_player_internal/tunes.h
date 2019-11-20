#ifndef _tunes
#define _tunes
// every file is converted to array that is stored in flashed memory.
// converter used is bin2c that can be found at https://sourceforge.net/projects/bin2c/files/

uint8_t DEFAULT_SONG = 0; // 0 is automatic, from sid header, any other value is tune number

///////////////////////////////////////////////  S I D     F I L E S /////////////////////////////////////////////////////////////
// uncomment only one
////////////////////////////////////////////////////////////////////////////////
//f401 - sids occupy memory less then 61440 bytes (and must end before $f400 ) (most of them don't work with f103)
////////////////////////////////////////////////////////////////////////////////
//#include"tunes/f401/RoboCop_3.h" //
//#include"tunes/f401/Choo_Choo_Blues.h" //
//#include"tunes/f401/North_and_South.h" // tunes that works: 1,46 47
//#include"tunes/f401/Lemmings.h" //
//#include"tunes/f401/house.h" // filter tester (Fanta really likes filters, unlike SID in my c64)
//#include"tunes/f401/Nobby_the_Aardvark.h"
//#include"tunes/f401/Last_Ninja_2.h" // on f103, all tunes (0-12) are working
//#include"tunes/f401/Turrican_2-The_Final_Fight.h" // on f103, tunes 1,2,3,4,5,6,7 are working (0 and 8 don't)
//#include"tunes/f401/R-Type.h"  // only tunes 0,1,2,3 are working on f103
//#include"tunes/f401/Katakis.h"
//#include"tunes/f401/Ghosts_n_Goblins_Arcade.h" // 34 tunes/sound effects. (on f103 plays only tune 0)
//#include"tunes/f401/Ghouls_n_Ghosts.h" // relaxing

/////////////////////////////////////////////////////////////////////////////////////
//f103 - sids less then 14500 bytes (and not using using memory outside it's memory space )
////////////////////////////////////////////////////////////////////////////////        good

#include"tunes/f103/Smold.h" // // MUSICIANS\M\Mahoney\Smold.sid  // sound good
//#include"tunes/f103/Alien.h" //  // /MUSICIANS/C/Clansey_Paul/Alien.sid $9088-$9263 Init: $908b Play: $90c2 // TODO: see what's wrong, works good with filters disabled. Bug in ENV3? Or ADSR? Or just bad filter emulation?
//#include"tunes/f103/Out4Just.h" // \MUSICIANS\J\Jonny\Out4Just.sid 03:37 // good for combined waveforms/noise testing
//#include"tunes/f103/Lazy_Jones_remix.h" // MUSICIANS\M\Mahoney\Lazy_Jones_remix.sid
//#include"tunes/f103/Blond_Blubber.h" //
//#include"tunes/f103/Deep.h" // good for resonance testing

// Rowlands_Steve
//#include"tunes/f103/Cyberdyne_Warrior.h"
//#include"tunes/f103/Retrograde.h"
//#include"tunes/f103/Creatures.h"
//#include"tunes/f103/Creatures_2.h" // not quite, but very close
//#include"tunes/f103/Mayhem_in_Monsterland.h" // need to check 1st tune (and test others. 2nd tune plays ok)


//#include"tunes/f103/Coma_Light_VIII_part_11.h"
//#include"tunes/f103/house.h"  // ripped directly from c64 and added sid header (only init/play address). only 1 tune
//#include"tunes/f103/Resolution_extended_version.h"
//#include"tunes/f103/Supremacy.h" // good for adsr and Pulse testing, no filters
//#include"tunes/f103/Coma_Light_13_tune_2.h"
//#include"tunes/f103/Return.h"
//#include"tunes/f103/Weebles.h" // cute
//#include"tunes/f103/Sanxion_Loader_Mix.h"
//#include"tunes/f103/Kata_Sandom.h" // this tune inspired the adventure "from C64 to BlueSID"
//#include"tunes/f103/Mac_Intro.h"
//#include"tunes/f103/Jani_Intro_v10.h"
//#include"tunes/f103/Sanxion.h" // not perfect
//#include"tunes/f103/Cotune_2.h" //  MUSICIANS\J\JCH\Cotune_2.sid - 01:52 - 13649bytes
//#include"tunes/f103/Booze_Your_Illusion_tune_5.h"  // MUSICIANS\T\Twynn\Booze_Your_Illusion_tune_5.sid
//#include"tunes/f103/R-Type_Demo.h" //
//#include"tunes/f103/Spiral_of_Arpeggioes.h" //
//#include"tunes/f103/Equalizer.h" // $8000 load address
//#include"tunes/f103/Legend_of_Kage.h"
//#include"tunes/f103/Game_Music.h" //
//#include"tunes/f103/BubbleBobble.h"  // remix
//#include"tunes/f103/Bubble_Bobble.h" //

//#include"tunes/f103/Voyage.h"
//#include"tunes/f103/Recovering_Heart.h" // \MUSICIANS\J\Jonny\Recovering_Heart.sid //  Giana sisters remix
//#include"tunes/f103/Lazy_Jones.h"
//#include"tunes/f103/TimeTrax.h" //  /MUSICIANS/C/Clansey_Paul/TimeTrax.sid $9400-$9FFF  Init: $9de2 Play: $9daf //

//#include"tunes/f103/Random_2.h"


//#include"tunes/f103/Elite.h" // info from sid say it is 50Hz player, but it is kinda slow. More like 55 Hz

//#include"tunes/f103/Coma_Light_13_tune_3.h" // not bad, but must to look into ADSR again, something is not right
//#include"tunes/f103/Comaland_tune_4.h" //
//#include"tunes/f103/3DEH.h" //
//#include"tunes/f103/Infusion.h" //
//#include"tunes/f103/Plush2K.h" //
//#include"tunes/f103/Red_Alert.h" //


//#include"tunes/f103/CJs_Elephant_Antics.h" //

//#include"tunes/f103/Ocean_Loader_1.h" //
//#include"tunes/f103/Ocean_Loader_2.h" //

//#include"tunes/f103/Monty_on_the_Run.h"
//#include"tunes/f103/Auf_Wiedersehen_Monty.h"

//#include"tunes/f103/International_Karate.h"
//#include"tunes/f103/Commando.h"
//#include"tunes/f103/Wiz.h"
//#include"tunes/f103/Master_of_Magic.h" // TODO:
//#include"tunes/f103/Spellbound.h" //

//#include"tunes/f103/Toxyc_Taste.h" // need filters
//#include"tunes/f103/Beyond_the_Forbidden_Forest.h" // simple, some tunes are good for high-frequency testing

//Tel_Jeroen
//#include"tunes/f103/Gaplus.h" //

///////////////////////////////////////////////////////////////////////////////         almoust sound like SID

//#include"tunes/f103/Scene_plus_III.h" //
//#include"tunes/f103/Spellbound_Dizzy.h" // looks ok, but use lot of irq time (actually, it need lot of cycles per rastertime for 6502 emulation) - good for "worst case" testing
//#include"tunes/f103/Magic_Land_Dizzy.h" // it plays, but not perfect
//#include"tunes/f103/No_Digis.h" //
//#include"tunes/f103/Popcorn_2.h" // MUSICIANS\B\Bakewell_Dwayne\Popcorn_2.sid - 3:13 -
//#include"tunes/f103/Razor_Crackzak_100.h"  //MUSICIANS\C\Crowley_Owen\Razor_Crackzak_100.sid // need to compare
//#include"tunes/f103/Competition.h"   //MUSICIANS\C\Cane\Competition.sid // i remember this tune when i was a kid
//#include"tunes/f103/Party_Hard.h" //
//#include"tunes/f103/Creatures_2_preview.h"
//#include"tunes/f103/Fields_of_Goats.h"
//////////////////////////////////////////////////////////////////////////////// there is a sound... but need adjustment in code

//#include"tunes/f103/Mexico.h"  // it's 5xspeed tune. barelly keeping up, even overclocked
//#include"tunes/f103/Defender_of_the_Crown.h" // 8 x speed player . Multiplier 64 on 72Mhz, 24 on 128Mhz to play normally


///////////////////////////////////////////////////////////////////////////////// doesn't work

//#include"tunes/f103/Navy_Moves.h" //
//#include"tunes/f401/Turrican_3.h" //
//#include"tunes/f103/DRAX_Tune.h" //

////////////////////////////////////////////////////////////////////////////// test sids
//#include"tunes/f103/delaybug.h"
//#include"tunes/f103/sndstarttest.h"
//#include"tunes/f103/sawscaletest.h"
//#include"tunes/f103/sawclaritest.h"
//#include"tunes/f103/resoscaletst.h"
//#include"tunes/f103/resodistest.h"
//#include"tunes/f103/pulseclartst.h"
//#include"tunes/f103/fltphasetest.h"
//#include"tunes/f103/filtertest.h"
//#include"tunes/f103/cutoffcurve.h"
//#include"tunes/f103/ctfcurvedsaw.h"
//#include"tunes/f103/combwformtst.h"
//#include"tunes/f103/noisewfsweep.h" // channel 1 noise from 0000 to ffff
//#include"tunes/f103/dactest.h" // TODO
//#include"tunes/f103/adsrtest.h" //TODO


#endif
