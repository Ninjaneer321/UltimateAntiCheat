#pragma once
#include <tchar.h>
#include <windows.h>
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <mscat.h>
#include "../Common/Logger.hpp"
#include "../Environment/Services.hpp"

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

using namespace std;

typedef struct
{
	LPWSTR lpszProgramName;
	LPWSTR lpszPublisherLink;
	LPWSTR lpszMoreInfoLink;
} SPROG_PUBLISHERINFO, * PSPROG_PUBLISHERINFO;

namespace Authenticode
{
	BOOL VerifyEmbeddedSignature(__in const LPCWSTR pwszSourceFile, __in const  BOOL checkRevoked);
	BOOL VerifyCatalogSignature(__in const LPCWSTR filePath, __in const  BOOL checkRevoked);
	BOOL HasSignature(__in const LPCWSTR filePath, __in const BOOL checkEndCertRevoked);

	wstring GetSignerFromFile(const wstring& filePath); //get the 'subject' field of the certifcate (often the company which published the software file)

	//https://learn.microsoft.com/en-us/previous-versions/troubleshoot/windows/win32/get-information-authenticode-signed-executables
	BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo, PSPROG_PUBLISHERINFO Info);
	BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME* st);
	wstring GetCertificateSubject(PCCERT_CONTEXT pCertContext);
	BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO* pCounterSignerInfo);
	LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);
}
