#!/bin/sh
#================================================================================ 
# 系統名稱 : FIX通訊作業平台 
# 程式名稱 : Get_Ffile.sh                          
# 作    者 : Ray
# 程式功能 : 發出request給Tandem傳送F03, F05, F11, F12, F13
#  
# 維護紀錄 : 
# 維護時間    維護人   版本       修改內容 
# ========== ======== =======  ==================================================
# 2011-12-29  miguel   v2.0     新格式調整     
# 2013-03-05  miguel   v3.0     調整執行內容  
# 
# 
# 程式介面 : 
# 介面種類   介面名稱        介面說明       
# =========  =============== ==================================================== 
# 
#
# 說明:  發出request給Tandem傳送F03, F05, F11, F12, F13
#	 此行程為單獨行程不需發出 Open 訊息
#
# 程式相依性說明 : 
# 
#     相依關鍵字為: ${GROUP}_Get_Ffile_ok
# 
#================================================================================ 
#************************************
#         共用變數定義區            *
#************************************

### get shell path

SHELL_PATH=`dirname $0`
RUN_PATH=`echo ${SHELL_PATH} | sed 's/\/.*//g'`
if [ "${RUN_PATH}" = "." ]
then
	RUN_PATH=`echo ${SHELL_PATH} | sed 's/\.//g'`
	SHELL_PATH="`pwd`${R_PATH}"
else
	if [ "${RUN_PATH}" = ".." ]
	then
		RUN_PATH=`echo ${SHELL_PATH} | sed 's/\.\.//g'`
		SHELL_PATH=`cd .. ; pwd`${R_PATH}
	fi
fi
SHELL_PATH=`echo ${SHELL_PATH} | sed 's/shell.*/shell/g'`

###   global set

GLOBAL_VAR="${SHELL_PATH}/set/SetGlobalVar.cp"
. $GLOBAL_VAR

#************************************
#         區域變數定義區            *
#************************************

###  help Usage ( -h )

	PRG="Get_Ffile"
	PRGX=`echo ${PRG} | sed 's/_Action//g' `
	USAGE='usage:  default => Group_ID=9030'

	USAGEA="${PRG}.sh '[Group_ID]' '[SEQ]'"
	USAGEB="${PRGX}_[Group_ID].sh  '[SEQ]'"

	USAGE="\n ${USAGE}\n\n\t${USAGEA}\n\t${USAGEB}\n"

###  get shell arg
. $G_CP_FUN_PATH/Get_Shell_Arg.cp

GROUP="${ARG_1}"        ## Group_ID
VARM_S="${ARG_2}"       ## SEQ

## default set
if [ "${GROUP}" = "" ]
then
	GROUP="9030"
fi

###  get Group name
. $G_CP_FUN_PATH/Group_Action.cp
Group_Name_Get "${GROUP}"

VARM_M="${GROUP_NAME}"
COMM_MSG="${VARM_S} 重送 F03-13 檔案"
DEPENDENT_INFORMATION="${GROUP}_Get_Ffile_ok"

###    status message

MSG_STAR="${COMM_MSG} 開始"
MSG_OK="${COMM_MSG}"';完成'
MSG_ERR="${COMM_MSG}"';警告, '

###   SHELL message

SHELL_STAR="${VARM_M}/${MSG_STAR}"
SHELL_OK="${VARM_M}/${MSG_OK}"
SHELL_ERR="${VARM_M}/${MSG_ERR}"

#************************************
#        共用函式定義區             *
#************************************

. $G_CP_FUN_PATH/Echo_Message.cp

#************************************
#	      MAIN		    *
#************************************

${G_ECHO_MSG} "" "${MSG_STAR}" "" "${SHELL_STAR}"

### check path
if [ ! -d ${G_ULOAD_PATH} ]
then
	mkdir -p ${G_ULOAD_PATH}
fi

### start process echo reguest to ${G_ULOAD_FILE}

TIME=$(date +%H%M%S)
echo "${TIME}${G_MRT}"MF03 >> ${G_ULOAD_FILE}
sleep 5
TIME=$(date +%H%M%S)
echo "${TIME}${G_MRT}"MF05 >> ${G_ULOAD_FILE}
sleep 5
TIME=$(date +%H%M%S)
echo "${TIME}${G_MRT}"MF11 >> ${G_ULOAD_FILE}
sleep 5
TIME=$(date +%H%M%S)
echo "${TIME}${G_MRT}"MF12 >> ${G_ULOAD_FILE}
sleep 5
TIME=$(date +%H%M%S)
echo "${TIME}${G_MRT}"MF13 >> ${G_ULOAD_FILE}

######  shell 結束

${G_ECHO_MSG} " ==> 寫入 ${G_ULOAD_FILE} file ok !" "${MSG_OK}" "" "${SHELL_OK}"
echo ${DEPENDENT_INFORMATION}                                             
exit 0
