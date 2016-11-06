#!/bin/bash 
# DISCLAIMER : original script came from https://gist.github.com/rodw/3073987
# This version was modified to backup either a User or an Organization's repos

# NOTE: if you have more than 100 repositories, you'll need to step thru the list of repos 
# returned by GitHub one page at a time, as described at https://gist.github.com/darktim/5582423


#-----Read arguments
while [[ $# -gt 1 ]]
do
	key="$1"

	case $key in
		-u|--usermode)		#usermode (organization or user)
		USERMODE="$2"
		if [[ $USERMODE != [ou] ]]; then
			echo "usermode must be 'o' (organization) or 'u' (user)"
			exit
		fi
		shift
		;;
		-d|--backupdir)		#backup directory
		GHBU_BACKUP_DIR="$2"
		if [ ! -d $GHBU_BACKUP_DIR ]; then
			echo "backup directory not valid"
			exit
		fi
		shift
		;;
		*)					#unknown option
		echo "unknown option ${1} with value ${2}"
		;;
	esac
	shift # past argument or value
done
#if [[ -n $1 ]]; then
#    echo "Last line of file specified as non-opt/last argument:"
#    tail -1 $1
#fi


#-----Ask for input
if [ $USERMODE == "o" ]; then
	read -p "Organization : " GHBU_ORG	# the GitHub organization/user whose repos will be backed up
fi
read -p "Username : " GHBU_UNAME	# the username of a GitHub account (to use with the GitHub API)		
read -s -p "Password : " GHBU_PASSWD		# the passsword for that account 
echo ""
if [ -z "$GHBU_BACKUP_DIR" ]; then
	read -p "Backup directory : " GHBU_BACKUP_DIR	# the backup directory
	while [ ! -d $GHBU_BACKUP_DIR ]
	do
		echo "Not a directory!"
		read -p "Backup directory : " GHBU_BACKUP_DIR
	done
fi


#-----Define other variables
GHBU_GITHOST=${GHBU_GITHOST-"github.com"}				# the GitHub hostname (see comments)
GHBU_PRUNE_OLD=${GHBU_PRUNE_OLD-true}					# when true, old backups will be deleted
GHBU_PRUNE_AFTER_N_DAYS=${GHBU_PRUNE_AFTER_N_DAYS-3}	# the min age (in days) of backup files to delete
GHBU_SILENT=${GHBU_SILENT-false}						# when true, only show error messages 
GHBU_API=${GHBU_API-"https://api.github.com"}			# base URI for the GitHub API
GHBU_GIT_CLONE_CMD="git clone --quiet --mirror git@${GHBU_GITHOST}:"	# base command to use to clone GitHub repos
TSTAMP=`date "+%Y%m%d-%H%M"`


# The function `check` will exit the script if the given command fails.
function check {
  "$@"
  status=$?
  if [ $status -ne 0 ]; then
    echo "ERROR: Encountered error (${status}) while running the following:" >&2
    echo "           $@"  >&2
    echo "       (at line ${BASH_LINENO[0]} of file $0.)"  >&2
    echo "       Aborting." >&2
    exit $status
  fi
}

# The function `tgz` will create a gzipped tar archive of the specified file ($1) and then remove the original
function tgz {
   check tar zcf $1.tar.gz $1 && check rm -rf $1
}

$GHBU_SILENT || (echo "" && echo "--- INITIALIZING ---" && echo "")

$GHBU_SILENT || echo "Using backup directory $GHBU_BACKUP_DIR"
check mkdir -p $GHBU_BACKUP_DIR

$GHBU_SILENT || echo -n "Fetching list of repositories for ${GHBU_ORG}..."

if [ $USERMODE == "o" ]; then
	# Repo list for organization
	REPOLIST=`check curl --silent -u $GHBU_UNAME:$GHBU_PASSWD ${GHBU_API}/orgs/${GHBU_ORG}/repos\?per_page=100 -q | check grep "\"name\"" | check awk -F': "' '{print $2}' | check sed -e 's/",//g'`
else
	# Repo list for user
	# NOTE: added affiliation=owner option to backup only my own repos
	REPOLIST=`check curl --silent -u $GHBU_UNAME:$GHBU_PASSWD ${GHBU_API}/user/repos\?affiliation=owner -q | check grep "\"name\"" | check awk -F': "' '{print $2}' | check sed -e 's/",//g'`	
fi


$GHBU_SILENT || echo "found `echo $REPOLIST | wc -w` repositories."
$GHBU_SILENT || (echo "" && echo "--- BACKING UP ---" && echo "")

for REPO in $REPOLIST; do
   $GHBU_SILENT || echo "Backing up ${GHBU_ORG}/${REPO}"
   check ${GHBU_GIT_CLONE_CMD}${GHBU_ORG}/${REPO}.git ${GHBU_BACKUP_DIR}/${GHBU_ORG}-${REPO}-${TSTAMP}.git && tgz ${GHBU_BACKUP_DIR}/${GHBU_ORG}-${REPO}-${TSTAMP}.git

   $GHBU_SILENT || echo "Backing up ${GHBU_ORG}/${REPO}.wiki (if any)"
   ${GHBU_GIT_CLONE_CMD}${GHBU_ORG}/${REPO}.wiki.git ${GHBU_BACKUP_DIR}/${GHBU_ORG}-${REPO}.wiki-${TSTAMP}.git 2>/dev/null && tgz ${GHBU_BACKUP_DIR}/${GHBU_ORG}-${REPO}.wiki-${TSTAMP}.git

   $GHBU_SILENT || echo "Backing up ${GHBU_ORG}/${REPO} issues"
   check curl --silent -u $GHBU_UNAME:$GHBU_PASSWD ${GHBU_API}/repos/${GHBU_ORG}/${REPO}/issues -q > ${GHBU_BACKUP_DIR}/${GHBU_ORG}-${REPO}.issues-${TSTAMP} && tgz ${GHBU_BACKUP_DIR}/${GHBU_ORG}-${REPO}.issues-${TSTAMP}
done

if $GHBU_PRUNE_OLD; then
  $GHBU_SILENT || (echo "" && echo "--- PRUNING ---" && echo "")
  $GHBU_SILENT || echo "Pruning backup files ${GHBU_PRUNE_AFTER_N_DAYS} days old or older."
  $GHBU_SILENT || echo "Found `find $GHBU_BACKUP_DIR -name '*.tar.gz' -mtime +$GHBU_PRUNE_AFTER_N_DAYS | wc -l` files to prune."
  find $GHBU_BACKUP_DIR -name '*.tar.gz' -mtime +$GHBU_PRUNE_AFTER_N_DAYS -exec rm -fv {} > /dev/null \; 
fi

$GHBU_SILENT || (echo "" && echo "--- DONE ---" && echo "")
$GHBU_SILENT || (echo "GitHub backup completed." && echo "")
