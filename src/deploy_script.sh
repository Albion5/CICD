#!/bin/bash

# Source environment variables
source deploy_script.env

# Enable debug mode
set -x

# Validate required variables
if [[ -z $REMOTE_USER_ || -z $REMOTE_IP_ || -z $REMOTE_DIR_ || -z $SSHPASS_ ]]; then
    echo "Error: Missing required environment variables. Please check deploy_script.env."
    exit 1
fi

# Assign variables
REMOTE_USER=$REMOTE_USER_
REMOTE_IP=$REMOTE_IP_
REMOTE_DIR=$REMOTE_DIR_
SSHPASS=$SSHPASS_

# Copy files
sshpass -p $SSHPASS scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null "$CAT_DIR"/build/s21_cat "$GREP_DIR"/build/s21_grep $REMOTE_USER@$REMOTE_IP:$REMOTE_DIR

# Log the status
if [ $? -eq 0 ]; then
    echo "Files copied successfully."
else
    echo "Error: Failed to copy files."
fi
