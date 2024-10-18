#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#pragma comment( lib, "WSock32.lib")

/// <summary>
/// サーバー関数
/// </summary>
void ChatServer(void) {
	// 通信を行うためのソケット
	SOCKET listen_s{};
	SOCKET s{};
	// IPアドレス等を格納
	SOCKADDR_IN saddr{};
	SOCKADDR_IN from{};
	// Accept待機用変数
	int fromlen{};
	// ポート情報変数
	u_short uport{};

	// ポート番号の入力
	printf("使用するポート番号 --> "); // ポート番号の入力を促す
	scanf_s("%hd", &uport);			// ポート番号の入力を取得
	fflush(stdin);

	/// ソケットを開く
	listen_s = socket(AF_INET, SOCK_STREAM, 0);
	// ソケットを開けていない場合はエラー
	if (listen_s == INVALID_SOCKET) {
		// ログ出力
		printf("ソケットオープン失敗\n");
		// クリーンアップ
		WSACleanup();
		// 終了
		return;
	}
	// 成功時はログに出力
	printf("ソケットオープン成功\n");

	// ソケットに名前を付ける
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	// saddrの設定
	saddr.sin_family		= AF_INET;
	saddr.sin_port			= htons(uport);
	saddr.sin_addr.s_addr	= INADDR_ANY;

	// バインドの成否確認
	if (bind(listen_s, (SOCKADDR*)&saddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		printf("Bindに失敗しました。\n");
		// ソケットを閉じる
		closesocket(listen_s);
		// 以降の処理を無視
		return;
	}

	// 成功した場合ログに出力
	printf("bindに成功しました。\n");

	// クライアント空の接続待ち状態にできているか確認
	if (listen(listen_s, 0) == SOCKET_ERROR) {
		printf("Listernに失敗しました。\n");
		// ソケットを閉じる
		closesocket(listen_s);
		// 以降の処理を無視
		return;
	}

	// 成功した場合ログに出力
	printf("listenに成功しました。\n");

	// 接続まで待機する
	printf("Acceptまで待機します . . .\n");
	fromlen = (int)sizeof(from);

	// Accept関数を実行
	s = accept(listen_s, (SOCKADDR*)&from, (int*)&listen_s);
	// Acceptの成否確認
	if (s == INVALID_SOCKET) {
		printf("Acceptに失敗しました。");
		// ソケットを閉じる
		closesocket(listen_s);
		// 以降の処理を無視
		return;
	}

	// 接続成功、ログに出力
	printf("%sが接続してきました。\n", inet_ntoa(from.sin_addr));
	printf("Acceptに成功しました。\n");

	// listenソケットを破棄する
	closesocket(listen_s);

	// 会話開始
	printf("会話開始\n");

	// 会話処理
	while (1)
	{
		// 文字列の終端
		int nRcv;
		// 受信する文字列
		char szBuf[1024];

		// データ受信
		nRcv = recv(s, szBuf, sizeof(szBuf) - 1, 0);
		szBuf[nRcv] = '\0';

		// 受信した文字列を表示
		printf("受信 --> %s\n", szBuf);
		printf("送信 --> ");

		// 送信する文字列を入力
		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		// 送信
		send(s, szBuf, (int)strlen(szBuf), 0);
	}
	
	// 終了時ソケットを閉じる
	closesocket(s);
}

/// <summary>
/// クライアント関数
/// </summary>
void ChatClient() {
	// ソケット
	SOCKET s{};
	// IPアドレス等を格納
	SOCKADDR_IN saddr{};
	// ポート情報変数
	u_short uport{};
	// サーバーから受け取る文字列
	char szServer[1024] = { 0 };
	unsigned int  addr;

	// ポート番号の入力
	printf("使用するポート番号 --> "); // ポート番号の入力を促す
	scanf_s("%hd", &uport);			// ポート番号の入力を取得
	fflush(stdin);

	// サーバー名、またはIPアドレスの入力
	printf("IPアドレス --> ");
	scanf_s("%s", szServer, 1024);
	fflush(stdin);

	/// ソケットを開く
	s = socket(AF_INET, SOCK_STREAM, 0);
	// ソケットを開けていない場合はエラー
	if (s == INVALID_SOCKET) {
		// ログ出力
		printf("ソケットオープン失敗\n");
		// 終了
		return;
	}
	
	// サーバーを名前で取得する
	HOSTENT* lpHost;
	lpHost = gethostbyname(szServer);

	// 名前で取得出来なかった場合IPアドレスで取得する
	if (lpHost == NULL) {
		addr = inet_addr(szServer);
		lpHost = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	// saddrの設定
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = lpHost->h_addrtype;
	saddr.sin_port = htons(uport);
	saddr.sin_addr.s_addr = *((u_long*)lpHost->h_addr);

	// クライアントのソケットをサーバーに接続
	if (connect(s, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
		printf("サーバー接続を確立できませんでした。\n");
		// ソケットを閉じる
		closesocket(s);
		// 終了
		return;
	}

	// 接続成功、ログに出力
	printf("サーバー接続を確立しました。\n");

	// サーバー側とのやり取りを開始
	while (1) {
		// 文字列の終端
		int nRcv;
		// 送信する文字列
		char szBuf[1024];

		// 送信する文字列を入力
		printf("送信 --> ");
		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		// 送信
		send(s, szBuf, (int)strlen(szBuf), 0);

		// データ受信
		nRcv		= recv(s, szBuf, sizeof(szBuf) - 1, 0);
		szBuf[nRcv] = '\0';
		// 受信した文字列を表示
		printf("受信 --> %s\n", szBuf);
	}

	// ソケットを閉じる
	closesocket(s);
}

int main() {
	// 一時変数
	WSADATA wsaData;
	int		mode;

	// 初期化失敗時はエラー
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		// ログに出力
		printf("WinSockの初期化に失敗しました\n");
		return 1;
	}

	// 入力によってサーバー、またはクライアントのどちらを実行するか指定
	printf("サーバー起動 : 0 || クライアント起動 : 1 を入力してください --> ");
	scanf_s("%d", &mode);
	fflush(stdin);

	// 処理を切り替え
	switch (mode)
	{
	case 0: // サーバー起動
		ChatServer();
	case 1: // クライアント起動
		ChatClient();
	default:
		printf("0 か 1 を入力してください\n");
		break;
	}

	//　クリーンアップ
	WSACleanup();

	// 終了
	return 0;
}