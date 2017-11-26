#ifndef LTFSDEFINE
#define LTFSDEFINE

//From IBM LTFS 2.2.1.0
#define LTFS_FILENAME_MAX 255

#define TAPE_MODEL_NAME_LEN_MAX     (16)
#define TAPE_VENDOR_NAME_LEN_MAX    (8)
#define TAPE_REVISION_CODE_LEN_MAX  (4)
#define TAPE_VENDORUNQ_DATA_LEN_MAX (20)
#define TAPE_DEVNAME_LEN_MAX        (1023)
#define TAPE_SERIAL_LEN_MAX         (32)

//From IBM LTFS 2.2.1.0 tape_drivers.h
#define VENDOR_ID_LENGTH    8
#define PRODUCT_ID_LENGTH   16
#define PRODUCT_NAME_LENGTH (PRODUCT_ID_LENGTH + 3) /* " [PRODUCT_ID]" */
#define PRODUCT_REV_LENGTH  4

//From IBM LTFS 2.2.1.0 tape_ops.h
struct tc_drive_info {
	char name[TAPE_DEVNAME_LEN_MAX + 1];           /* Device name like "/dev/IBMtape0" */
	char vendor[TAPE_VENDOR_NAME_LEN_MAX + 1];     /* Vendor code "IBM" */
	char model[TAPE_MODEL_NAME_LEN_MAX + 1];       /* Device identifier */
	char serial_number[TAPE_SERIAL_LEN_MAX + 1];   /* Serial number of the drvice */
	char product_name[PRODUCT_NAME_LENGTH + 1];    /* Product name like " [ULTRIUM-TD5]" */
};



//TAPE SERIAL must be 6 char
#define LTFS_TAPESERIAL_CHARNUM 6
#define LTFS_REFERENCE_IBM 0 //Reference by IBM(include OEM)
#define LTFS_REFERENCE_HP  1 //Reference by HP(include OEM)

#define LTFS_COMMAND_PYTHON "python"
#define LTFS_COMMAND_ADMINTOOL """C:\\Program Files\\IBM\\LTFS\\ltfsadmintool"""
#define LTFS_COMMAND_ARGS_I      "-i"
#define LTFS_COMMAND_ARGS_T      "-t"
#define LTFS_COMMAND_ARGS_M      "-m"
#define LTFS_COMMAND_ARGS_F      "-f"
#define LTFS_COMMAND_ARGS_HYPHENHYPEHN "--"
#define LTFS_COMMAND_ARGS_N      "-n"
#define LTFS_COMMAND_ARGS_HOME   "homeslot"

#define LTFS_COMMAND_LTFS	"ltfs"
#define LTFS_COMMNAD_LTFS_ARGS_O "-o"
#define LTFS_COMMNAD_LTFS_ARGS_O_DEVNAME "changer_devname="
#define LTFS_COMMNAD_LTFS_ARGS_O_SYNCTYPE "sync_type="
#define LTFS_COMMNAD_LTFS_ARGS_O_UMOUNTSYNC "unmount"
#define LTFS_COMMNAD_LTFS_ARGS_O_SYNCTIME "time@"
#define LTFS_COMMNAD_LTFS_ARGS_O_WORKDIR "work_directory="

#define LTFS_COMMAND_LTFSUNMOUNT "ltfsunmount"

#define LTFSLE_MOUNT_REG_PATTERN "[A-Z]"
#define LTFSLE_SCSIID_REG_PATTERN "[0-9]\\.[0-9]\\.[0-9]\\.[0-9]"

#define LTOLOADER_NUMBER_MAX 8 // It is better to change according to the number of cartridges.

#endif // LTFSDEFINE

