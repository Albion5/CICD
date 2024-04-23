#!/bin/bash

# Source environment variables
source tel_bot.env

# Validate required variables
if [[ -z $BOT_TOKEN || -z $USER_ID ]]; then
    echo "Error: Missing required environment variables. Please check tel_bot.env."
    exit 1
fi

# Assign variables
TELEGRAM_BOT_TOKEN=$BOT_TOKEN
TELEGRAM_USER_ID=$USER_ID
TIME=8
URL="https://api.telegram.org/bot$TELEGRAM_BOT_TOKEN/sendMessage"
TEXT="$CI_PROJECT_NAME%0A%0ASTAGE:+$CI_JOB_STAGE%0AJOB:+$CI_JOB_NAME%0ASTATUS:+$CI_JOB_STATUS%0AURL:+$CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/"

# Send Telegram message
response=$(curl -s --max-time $TIME -d "chat_id=$TELEGRAM_USER_ID&disable_web_page_preview=1&text=$TEXT" $URL)

# Check response
if [[ $? -eq 0 ]]; then
    echo "Telegram message sent successfully."
else
    echo "Error: Failed to send Telegram message."
    echo "Response: $response"
fi
