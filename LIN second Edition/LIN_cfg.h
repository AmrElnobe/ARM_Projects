/*
 * LIN_cfg.h
 *
 *  Created on: May 8, 2020
 *      Author: DELL
 */

#ifndef LIN_CFG_H_
#define LIN_CFG_H_
/*Options for Node State*/
#define MASTER_NODE  		1
#define SLAVE_NODE  		2

/*Options for msg state*/
#define LISTENER      		1
#define OWNER    		    2

/*Options for LIN version*/
#define CLASSIC_LIN         0
#define ENHANCED_LIN        1


#define LIN_VERSION         CLASSIC_LIN
#define NODE_STATE   		MASTER_NODE
#define NUMBER_OF_MSGS   	2

typedef struct
{
	uint_8t ID;
	uint_8t msgState;
	uint_8t dataSize;
	uint_8t * data;
	NotifyCbf_t MsgCbf;
}LIN_Slavecfg_t;

#if NODE_STATE == MASTER_NODE
#define MAX_MSGS_NUM    3
typedef struct
{
	uint_8t ID;
	uint_8t ExecTime;

}LIN_Mastercfg_t;
#endif

#endif /* LIN_CFG_H_ */
