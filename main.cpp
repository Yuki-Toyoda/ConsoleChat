#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#pragma comment( lib, "WSock32.lib")

/// <summary>
/// �T�[�o�[�֐�
/// </summary>
void ChatServer(void) {
	// �ʐM���s�����߂̃\�P�b�g
	SOCKET listen_s{};
	SOCKET s{};
	// IP�A�h���X�����i�[
	SOCKADDR_IN saddr{};
	SOCKADDR_IN from{};
	// Accept�ҋ@�p�ϐ�
	int fromlen{};
	// �|�[�g���ϐ�
	u_short uport{};

	// �|�[�g�ԍ��̓���
	printf("�g�p����|�[�g�ԍ� --> "); // �|�[�g�ԍ��̓��͂𑣂�
	scanf_s("%hd", &uport);			// �|�[�g�ԍ��̓��͂��擾
	fflush(stdin);

	/// �\�P�b�g���J��
	listen_s = socket(AF_INET, SOCK_STREAM, 0);
	// �\�P�b�g���J���Ă��Ȃ��ꍇ�̓G���[
	if (listen_s == INVALID_SOCKET) {
		// ���O�o��
		printf("�\�P�b�g�I�[�v�����s\n");
		// �N���[���A�b�v
		WSACleanup();
		// �I��
		return;
	}
	// �������̓��O�ɏo��
	printf("�\�P�b�g�I�[�v������\n");

	// �\�P�b�g�ɖ��O��t����
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	// saddr�̐ݒ�
	saddr.sin_family		= AF_INET;
	saddr.sin_port			= htons(uport);
	saddr.sin_addr.s_addr	= INADDR_ANY;

	// �o�C���h�̐��ۊm�F
	if (bind(listen_s, (SOCKADDR*)&saddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		printf("Bind�Ɏ��s���܂����B\n");
		// �\�P�b�g�����
		closesocket(listen_s);
		// �ȍ~�̏����𖳎�
		return;
	}

	// ���������ꍇ���O�ɏo��
	printf("bind�ɐ������܂����B\n");

	// �N���C�A���g��̐ڑ��҂���Ԃɂł��Ă��邩�m�F
	if (listen(listen_s, 0) == SOCKET_ERROR) {
		printf("Listern�Ɏ��s���܂����B\n");
		// �\�P�b�g�����
		closesocket(listen_s);
		// �ȍ~�̏����𖳎�
		return;
	}

	// ���������ꍇ���O�ɏo��
	printf("listen�ɐ������܂����B\n");

	// �ڑ��܂őҋ@����
	printf("Accept�܂őҋ@���܂� . . .\n");
	fromlen = (int)sizeof(from);

	// Accept�֐������s
	s = accept(listen_s, (SOCKADDR*)&from, (int*)&listen_s);
	// Accept�̐��ۊm�F
	if (s == INVALID_SOCKET) {
		printf("Accept�Ɏ��s���܂����B");
		// �\�P�b�g�����
		closesocket(listen_s);
		// �ȍ~�̏����𖳎�
		return;
	}

	// �ڑ������A���O�ɏo��
	printf("%s���ڑ����Ă��܂����B\n", inet_ntoa(from.sin_addr));
	printf("Accept�ɐ������܂����B\n");

	// listen�\�P�b�g��j������
	closesocket(listen_s);

	// ��b�J�n
	printf("��b�J�n\n");

	// ��b����
	while (1)
	{
		// ������̏I�[
		int nRcv;
		// ��M���镶����
		char szBuf[1024];

		// �f�[�^��M
		nRcv = recv(s, szBuf, sizeof(szBuf) - 1, 0);
		szBuf[nRcv] = '\0';

		// ��M�����������\��
		printf("��M --> %s\n", szBuf);
		printf("���M --> ");

		// ���M���镶��������
		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		// ���M
		send(s, szBuf, (int)strlen(szBuf), 0);
	}
	
	// �I�����\�P�b�g�����
	closesocket(s);
}

/// <summary>
/// �N���C�A���g�֐�
/// </summary>
void ChatClient() {
	// �\�P�b�g
	SOCKET s{};
	// IP�A�h���X�����i�[
	SOCKADDR_IN saddr{};
	// �|�[�g���ϐ�
	u_short uport{};
	// �T�[�o�[����󂯎�镶����
	char szServer[1024] = { 0 };
	unsigned int  addr;

	// �|�[�g�ԍ��̓���
	printf("�g�p����|�[�g�ԍ� --> "); // �|�[�g�ԍ��̓��͂𑣂�
	scanf_s("%hd", &uport);			// �|�[�g�ԍ��̓��͂��擾
	fflush(stdin);

	// �T�[�o�[���A�܂���IP�A�h���X�̓���
	printf("IP�A�h���X --> ");
	scanf_s("%s", szServer, 1024);
	fflush(stdin);

	/// �\�P�b�g���J��
	s = socket(AF_INET, SOCK_STREAM, 0);
	// �\�P�b�g���J���Ă��Ȃ��ꍇ�̓G���[
	if (s == INVALID_SOCKET) {
		// ���O�o��
		printf("�\�P�b�g�I�[�v�����s\n");
		// �I��
		return;
	}
	
	// �T�[�o�[�𖼑O�Ŏ擾����
	HOSTENT* lpHost;
	lpHost = gethostbyname(szServer);

	// ���O�Ŏ擾�o���Ȃ������ꍇIP�A�h���X�Ŏ擾����
	if (lpHost == NULL) {
		addr = inet_addr(szServer);
		lpHost = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	// saddr�̐ݒ�
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = lpHost->h_addrtype;
	saddr.sin_port = htons(uport);
	saddr.sin_addr.s_addr = *((u_long*)lpHost->h_addr);

	// �N���C�A���g�̃\�P�b�g���T�[�o�[�ɐڑ�
	if (connect(s, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR) {
		printf("�T�[�o�[�ڑ����m���ł��܂���ł����B\n");
		// �\�P�b�g�����
		closesocket(s);
		// �I��
		return;
	}

	// �ڑ������A���O�ɏo��
	printf("�T�[�o�[�ڑ����m�����܂����B\n");

	// �T�[�o�[���Ƃ̂������J�n
	while (1) {
		// ������̏I�[
		int nRcv;
		// ���M���镶����
		char szBuf[1024];

		// ���M���镶��������
		printf("���M --> ");
		scanf_s("%s", szBuf, 1024);
		fflush(stdin);

		// ���M
		send(s, szBuf, (int)strlen(szBuf), 0);

		// �f�[�^��M
		nRcv		= recv(s, szBuf, sizeof(szBuf) - 1, 0);
		szBuf[nRcv] = '\0';
		// ��M�����������\��
		printf("��M --> %s\n", szBuf);
	}

	// �\�P�b�g�����
	closesocket(s);
}

int main() {
	// �ꎞ�ϐ�
	WSADATA wsaData;
	int		mode;

	// ���������s���̓G���[
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		// ���O�ɏo��
		printf("WinSock�̏������Ɏ��s���܂���\n");
		return 1;
	}

	// ���͂ɂ���ăT�[�o�[�A�܂��̓N���C�A���g�̂ǂ�������s���邩�w��
	printf("�T�[�o�[�N�� : 0 || �N���C�A���g�N�� : 1 ����͂��Ă������� --> ");
	scanf_s("%d", &mode);
	fflush(stdin);

	// ������؂�ւ�
	switch (mode)
	{
	case 0: // �T�[�o�[�N��
		ChatServer();
	case 1: // �N���C�A���g�N��
		ChatClient();
	default:
		printf("0 �� 1 ����͂��Ă�������\n");
		break;
	}

	//�@�N���[���A�b�v
	WSACleanup();

	// �I��
	return 0;
}