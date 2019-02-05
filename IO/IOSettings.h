#ifndef IOSETTINGS_H_
#define IOSETTINGS_H_

/* Max buffer size for output messages */
#ifndef IO_OUTPUT_BUFFER_SIZE
#define IO_OUTPUT_BUFFER_SIZE 256
#endif /* IO_OUTPUT_BUFFER_SIZE */

/* Max buffer size for input messages */
#ifndef IO_INPUT_BUFFER_SIZE
#define IO_INPUT_BUFFER_SIZE 256
#endif /* IO_INPUT_BUFFER_SIZE */

/* Max number of new messages to store */
#ifndef IO_NEW_MESSAGES_BUFFER_SIZE
#define IO_NEW_MESSAGES_BUFFER_SIZE 8
#endif /* IO_NEW_MESSAGES_BUFFER_SIZE */

#ifndef IO_EOM_CHARACTERS
#define IO_EOM_CHARACTERS "\r\n"
#endif /* IO_EOM_CHARACTERS */

#endif /* IOSETTINGS_H_ */
