#! /usr/bin/env python
# -*- coding: utf-8 -*-
# ユーザーストリームで流れているアカウントの画像を片っ端から取得、保存。
# Perl の名刺化スクリプトと連携せよ。
# 2012 / 11 / 14
# jskny


# http://d.hatena.ne.jp/iacctech/20110429/1304090609


import sys, tweepy, urllib, urllib2
import os, time, subprocess, socket
import re


from tweepy.streaming import StreamListener, Stream
from tweepy.auth import OAuthHandler
from datetime import timedelta


# カラは、ローカルホストを意味する。
host = ''
port = 18385

serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# クライアントの接続を待つ
serversock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serversock.bind((host, port))
serversock.listen(1)
print 'Waiting for connections...'
clientsock, client_address = serversock.accept()
print 'Connection his Succeed...'


# それぞれ取得して埋めて下さい
consumer_key = "oDiVnBOqcYjie0T8AN6XyA"
consumer_secret = "0rsndWq3N3u8AJXKP7gfwrAcdwzPoFxAgZ5PuLt4Ww"
access_key = "397948629-j4HutoScDcL5ncMZNvuA13JY6BA3D2zEJyZPdEAJ"
access_secret = "N3UGJUwxDcrs0yz4mK3Y9cNhkw8IpO6kHnFIzHMH3pM"


def GetOauth():
	auth = OAuthHandler(consumer_key, consumer_secret)
	auth.set_access_token(access_key, access_secret)
	return auth


# アイコン保存くん
def SaveIcon(screen_name):
	flag = os.path.exists("%s/%s.png" % (os.path.dirname(os.path.abspath(__file__)), screen_name))
	if flag:
		# ファイルあり。スルー
		return;
	else:
		# ファイルなし。ダウンロード！！
		urllib.urlretrieve(('http://api.twitter.com/1/users/profile_image/%s' % screen_name), ('%s/%s.png' % (os.path.dirname(os.path.abspath(__file__)), screen_name)))


	return;


# Tweepy の機能で行きまーす。
class AbstractedlyListener(StreamListener):
	def on_status(self, status):
		try:
			# RT の時は、無視。
			if re.search("RT", status.text.encode("UTF-8")) != None :
				return


			# Ubuntuの時は気づかなかったんだけど、Windowsで動作確認してたら
			# created_atがUTC（世界標準時）で返ってきてた。
			# なので日本時間にするために9時間プラスする。
			status.created_at += timedelta(hours=9)

			print "%s" % status.text.encode("UTF-8")
			print "%s(%s) %s via %s\n" % (status.author.name.encode("UTF-8"),
				status.author.screen_name.encode("UTF-8"),
				status.created_at, status.source.encode("UTF-8"))
			SaveIcon(status.author.screen_name.encode("UTF-8"))

			# perl 実行！！！
			subprocess.call(("perl %s\CreateTweetMemo.pl \"%s\" \"%s\" \"%s\" \"%s.png\"" % (os.path.dirname(os.path.abspath(__file__)), status.author.screen_name.encode("UTF-8"), status.text.encode("UTF-8"), status.created_at, status.id_str.encode("UTF-8"))), shell=True)

			# 送信。
			ttt = "%s" % status.id_str.encode('UTF-8')
#			print ttt
			clientsock.sendall(str(ttt))
			time.sleep(0.3)


		except Exception, e:
			print >> sys.stderr, 'Encounted Exception:', e
			pass


	def on_error(self, status_code):
		print 'An error has occured! Status code = %s' % status_code
		return True # keep stream alive


	def on_timeout(self):
		print "UserStream is timeout..."


def main():
	auth = GetOauth()
	stream = Stream(auth, AbstractedlyListener(), secure=True)
	stream.userstream()


if __name__ == "__main__":
	try:
		main()
		clientsock.close()
		sys.exit()
	except KeyboardInterrupt:
		clientsock.close()
		sys.exit()


