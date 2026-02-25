#!/bin/bash
# 수정할 부분
LOCAL_DIR="/home/sb/PycharmProjects/Tank/"
REMOTE_TARGET="arduino@172.21.86.112:/home/arduino/Tank/"

echo "동기화 시작..."
# fswatch나 inotifywait를 사용하여 파일 변경 감지
while inotifywait -r -e modify,create,delete,move "$LOCAL_DIR"; do
    rsync -avz --delete "$LOCAL_DIR" "$REMOTE_TARGET"
    echo "서버로 동기화 완료: $(date)"
done
