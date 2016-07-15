while true

do

wget -q http://hiweeds.net/shadowsocks.json -P /tmp/
newMd5=$(md5sum /tmp/shadowsocks.json | cut -d ' ' -f1)
oldMd5=$(md5sum /etc/shadowsocks.json | cut -d ' ' -f1)

if [ "$newMd5" != "$oldMd5" ]; then
	mv /tmp/shadowsocks.json /etc/shadowsocks.json
	killall -9 ss-redir
	/etc/init.d/shadowsocks start
	rm /tmp/shadowsocks.json
fi

sleep 120
done
