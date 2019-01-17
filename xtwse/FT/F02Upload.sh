#!/bin/sh
#================================================================================ 
# �t�ΦW�� : FIX�q�T�@�~���x 
# �{���W�� : F02Upload.sh                               
# �@    �� : 
# �{���\�� : F02upload
#  
# ���@���� : 
# ���@�ɶ�    ���@�H   ����       �ק鷺�e 
# ========== ======== =======  ==================================================
# 2011-12-29  miguel   v2.0     �s�榡�վ�     
# 2013-03-05  miguel   v3.0     �վ���椺�e  
# 
# 
# �{������ : 
# ��������   �����W��        ��������       
# =========  =============== ==================================================== 
# 
#
# ����:  F02Upload �W�Ǵ��յ��G(F02)
#
# �{���̩ۨʻ��� : 
# 
#     �ۨ�����r��: ${GROUP}_F02upload_ok
# 
#================================================================================ 
#************************************
#         �@���ܼƩw�q��            *
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
#         �ϰ��ܼƩw�q��            *
#************************************

###  help Usage ( -h )

	PRG="F02Upload"
	PRGX=`echo ${PRG} | sed 's/_Action//g' `
	USAGE='usage:  default => Group_ID=1070'

	USAGEA="${PRG}.sh '[Group_ID]' '[SEQ]' "
	USAGEB="${PRG}_[Group_ID].sh '[SEQ]' "

	USAGE="\n ${USAGE}\n\n\t${USAGEA}\n\t${USAGEB}\n"

###  get shell user
. $G_CP_FUN_PATH/Get_Shell_Arg.cp

GROUP="${ARG_1}"        ## Group_ID
VARM_S="${ARG_2}"       ## SEQ

## default set
if [ "${GROUP}" = "" ]
then
	GROUP="1070"
fi

###  get Group name
. $G_CP_FUN_PATH/Group_Action.cp
Group_Name_Get "${GROUP}"

VARM_M="${GROUP_NAME}"
COMM_MSG="${VARM_S} �W�� F02 �u�����յ��G"
DEPENDENT_INFORMATION="${GROUP}_F02upload_ok"

###    status message

MSG_STAR="${COMM_MSG} �}�l"
MSG_OK="${COMM_MSG}"';����'
MSG_ERR="${COMM_MSG}"';ĵ�i, '

###   Open message

BASE_MSG="${G_MRT};`hostname`;${VARM_M};CronjobAgent;"
XA_OK="XA1004;${BASE_MSG}${MSG_OK}"

###   SHELL message

SHELL_STAR="${VARM_M}/${MSG_STAR}"
SHELL_OK="${VARM_M}/${MSG_OK}"
SHELL_ERR="${VARM_M}/${MSG_ERR}"

#************************************
#        �@�Ψ禡�w�q��             *
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

### process F02 upload
echo "${G_TIME}${G_MRT}"FF02 >> ${G_ULOAD_FILE}

######  shell ����

${G_ECHO_MSG} " ==> �g�J ${G_ULOAD_FILE} file ok !" "${MSG_OK}" "${XA_OK}" "${SHELL_OK}"
echo ${DEPENDENT_INFORMATION}                                             
exit 0
