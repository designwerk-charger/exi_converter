OFF="\033[m"
GREY="\033[90m"
BLUE="\033[34m"
CYAN="\033[36m"
YELLOW="\033[33m"

# for other colors see: https://misc.flogisoft.com/bash/tip_colors_and_formatting


function logLine {
    echo "$1"
}

function logInfo {
    echo -e ${CYAN}"=> $1"${OFF}
}

function logTip {
    echo -e ${YELLOW}"TIP:"${OFF}" $1"
}

function logCmd {
    echo -e ${GREY}"> $1"${OFF}
}
