#pragma once

enum CoreError
{
	Success,
	SocketInvalid,
	SocketAlreadyCreate,
	SocketFailCreate,
	SocketFailListen,

	ParameterNull,

	WsaError = 10000,
};