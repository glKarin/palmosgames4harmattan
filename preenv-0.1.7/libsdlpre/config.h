#ifndef CONFIG_H

/** Compile time configuration variables */

/** Version to print on informative messages */
#define LIB_VERSION			    "0.1.7"

/** Use the N900's MCE vibrator support */
#ifdef _KARIN_PRE //k
#define USE_MCE_VIBRA           1
#endif

/** Use the N900's liblocation GPS support */
#ifdef _KARIN_PRE //k
#define USE_LIBLOCATION         1
#endif

/** Max framerate when an application is paused */
#define DEACTIVATED_FPS_CAP     2

/** Size of the "task switcher" button on topleft corner. */
#define TASKNAV_BTN_WIDTH       112
#define TASKNAV_BTN_HEIGHT      56
/** Time the button has to be held to trigger a minimize event. */
#define TASKNAV_BTN_TIME        2

/** The file where the simulated device's unique ID is stored. */
#define UNIQUE_ID_FILE          "/home/user/.preenv-uid"
/** Where to get a random ID from. */
#define RANDOM_SRC_FILE         "/dev/random"
/** The amount of random bytes to read. */
#define UNIQUE_ID_RND_SIZE      24

/** User-writable path where "data files" (see PDL_GetDataFilePath) are stored. */
#define DATA_FILE_PATH          "/home/user/.preenv"
/** Permissions for the above directory, if we get to create it. */
#define DATA_FILE_PERM          0775

#endif
