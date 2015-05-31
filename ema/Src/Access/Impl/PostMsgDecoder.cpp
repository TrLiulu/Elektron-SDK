/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2015. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "PostMsgDecoder.h"
#include "ExceptionTranslator.h"
#include "StaticDecoder.h"
#include "rtr/rsslMsgDecoders.h"

using namespace thomsonreuters::ema::access;

PostMsgDecoder::PostMsgDecoder() :
 MsgDecoder(),
 _rsslMsg(),
 _pRsslMsg( 0 ),
 _name(),
 _extHeader(),
 _permission(),
 _serviceName(),
 _hexBuffer(),
 _serviceNameSet( false ),
 _rsslMajVer( RSSL_RWF_MAJOR_VERSION ),
 _rsslMinVer( RSSL_RWF_MINOR_VERSION ),
 _errorCode( OmmError::NoErrorEnum )
{
}

PostMsgDecoder::~PostMsgDecoder()
{
}

void PostMsgDecoder::setRsslData( UInt8 majVer, UInt8 minVer, RsslMsg* rsslMsg, const RsslDataDictionary* rsslDictionary )
{
	_serviceNameSet = false;

	_pRsslMsg = rsslMsg;

	_pRsslDictionary = rsslDictionary;

	_rsslMajVer = majVer;

	_rsslMinVer = minVer;

	StaticDecoder::setRsslData( &_attrib, &_pRsslMsg->msgBase.msgKey.encAttrib,
		hasAttrib() ? _pRsslMsg->msgBase.msgKey.attribContainerType : RSSL_DT_NO_DATA, majVer, minVer, _pRsslDictionary );

	StaticDecoder::setRsslData( &_payload, &_pRsslMsg->msgBase.encDataBody, _pRsslMsg->msgBase.containerType, majVer, minVer, _pRsslDictionary );
}

void PostMsgDecoder::setRsslData( UInt8 majVer, UInt8 minVer, RsslBuffer* rsslBuffer, const RsslDataDictionary* rsslDictionary, void* )
{
	_serviceNameSet = false;

	rsslClearMsg( &_rsslMsg );

	_pRsslMsg = &_rsslMsg;

	_pRsslDictionary = rsslDictionary;

	_rsslMajVer = majVer;

	_rsslMinVer = minVer;

	RsslDecodeIterator decodeIter;

	rsslClearDecodeIterator( &decodeIter );

	RsslRet retCode = rsslSetDecodeIteratorBuffer( &decodeIter, rsslBuffer );
	if ( RSSL_RET_SUCCESS != retCode )
	{
		_errorCode = OmmError::IteratorSetFailureEnum;
		return;
	}

	retCode = rsslSetDecodeIteratorRWFVersion( &decodeIter, _rsslMajVer, _rsslMinVer );
	if ( RSSL_RET_SUCCESS != retCode )
	{
		_errorCode = OmmError::IteratorSetFailureEnum;
		return;
	}

	retCode = rsslDecodeMsg( &decodeIter, _pRsslMsg );
	switch ( retCode )
	{
	case RSSL_RET_SUCCESS :
		_errorCode = OmmError::NoErrorEnum;
		break;
	case RSSL_RET_ITERATOR_OVERRUN :
		_errorCode = OmmError::IteratorOverrunEnum;
		break;
	case RSSL_RET_INCOMPLETE_DATA :
		_errorCode = OmmError::IncompleteDataEnum;
		break;
	default :
		_errorCode = OmmError::UnknownErrorEnum;
		break;
	}

	StaticDecoder::setRsslData( &_attrib, &_pRsslMsg->msgBase.msgKey.encAttrib,
		hasAttrib() ? _pRsslMsg->msgBase.msgKey.attribContainerType : RSSL_DT_NO_DATA, majVer, minVer, _pRsslDictionary );

	StaticDecoder::setRsslData( &_payload, &_pRsslMsg->msgBase.encDataBody, _pRsslMsg->msgBase.containerType, majVer, minVer, _pRsslDictionary );
}

void PostMsgDecoder::setRsslData( RsslDecodeIterator* , RsslBuffer* )
{
}

bool PostMsgDecoder::hasMsgKey() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_MSG_KEY ) ? true : false;
}

bool PostMsgDecoder::hasName() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_MSG_KEY ) &&
			( _pRsslMsg->msgBase.msgKey.flags & RSSL_MKF_HAS_NAME ) ? true : false;
}

bool PostMsgDecoder::hasNameType() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_MSG_KEY ) &&
			( _pRsslMsg->msgBase.msgKey.flags & RSSL_MKF_HAS_NAME_TYPE ) ? true : false;
}

bool PostMsgDecoder::hasServiceId() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_MSG_KEY ) &&
			( _pRsslMsg->msgBase.msgKey.flags & RSSL_MKF_HAS_SERVICE_ID ) ? true : false;
}

bool PostMsgDecoder::hasId() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_MSG_KEY ) &&
			( _pRsslMsg->msgBase.msgKey.flags & RSSL_MKF_HAS_IDENTIFIER ) ? true : false;
}

bool PostMsgDecoder::hasFilter() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_MSG_KEY ) &&
			( _pRsslMsg->msgBase.msgKey.flags & RSSL_MKF_HAS_FILTER ) ? true : false;
}

bool PostMsgDecoder::hasAttrib() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_MSG_KEY ) &&
			( _pRsslMsg->msgBase.msgKey.flags & RSSL_MKF_HAS_ATTRIB ) ? true : false;
}

bool PostMsgDecoder::hasPostId() const
{
	return rsslPostMsgCheckHasPostId( &_pRsslMsg->postMsg ) ? true : false;
}

bool PostMsgDecoder::hasPostUserRights() const
{
	return rsslPostMsgCheckHasPostUserRights( &_pRsslMsg->postMsg ) ? true : false;
}

bool PostMsgDecoder::hasPayload() const
{
	return _pRsslMsg->msgBase.containerType != RSSL_DT_NO_DATA ? true : false;
}

bool PostMsgDecoder::hasHeader() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_EXTENDED_HEADER ) ? true : false;
}

bool PostMsgDecoder::hasSeqNum() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_SEQ_NUM ) ? true : false;
}

bool PostMsgDecoder::hasPermissionData() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_PERM_DATA ) ? true : false;
}

bool PostMsgDecoder::hasPartNum() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_HAS_PART_NUM ) ? true : false;
}

bool PostMsgDecoder::hasServiceName() const
{
	return _serviceNameSet;
}

Int32 PostMsgDecoder::getStreamId() const
{
	return _pRsslMsg->msgBase.streamId;
}

UInt16 PostMsgDecoder::getDomainType() const
{
	return _pRsslMsg->msgBase.domainType;
}

const EmaString& PostMsgDecoder::getName() const
{
	if ( !hasName() )
	{
		EmaString temp( "Attempt to getName() while it is NOT set." );
		throwIueException( temp );
	}

	_name.setInt( _pRsslMsg->msgBase.msgKey.name.data, _pRsslMsg->msgBase.msgKey.name.length, false );

	return _name.toString();
}

UInt8 PostMsgDecoder::getNameType() const
{
	if ( !hasNameType() )
	{
		EmaString temp( "Attempt to getNameType() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->msgBase.msgKey.nameType;
}

UInt32 PostMsgDecoder::getServiceId() const
{
	if ( !hasServiceId() )
	{
		EmaString temp( "Attempt to getServiceId() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->msgBase.msgKey.serviceId;
}

Int32 PostMsgDecoder::getId() const
{
	if ( !hasId() )
	{
		EmaString temp( "Attempt to getId() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->msgBase.msgKey.identifier;
}

UInt32 PostMsgDecoder::getSeqNum() const
{
	if ( !hasSeqNum() )
	{
		EmaString temp( "Attempt to getSeqNum() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->postMsg.seqNum;
}

UInt32 PostMsgDecoder::getPublisherIdUserId() const
{
	return _pRsslMsg->postMsg.postUserInfo.postUserId;
}

UInt32 PostMsgDecoder::getPublisherIdUserAddress() const
{
	return _pRsslMsg->postMsg.postUserInfo.postUserAddr;
}

UInt32 PostMsgDecoder::getPostId() const
{
	if ( !hasPostId() )
	{
		EmaString temp( "Attempt to getPostId() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->postMsg.postId;
}

UInt16 PostMsgDecoder::getPartNum() const
{
	if ( !hasPartNum() )
	{
		EmaString temp( "Attempt to getPartNum() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->postMsg.partNum;
}

UInt16 PostMsgDecoder::getPostUserRights() const
{
	if ( !hasPostUserRights() )
	{
		EmaString temp( "Attempt to getPostUserRights() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->postMsg.postUserRights;
}

bool PostMsgDecoder::getSolicitAck() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_ACK ) ? true : false;
}

bool PostMsgDecoder::getComplete() const
{
	return ( _pRsslMsg->postMsg.flags & RSSL_PSMF_POST_COMPLETE ) ? true : false;
}

const EmaString& PostMsgDecoder::getServiceName() const
{
	if ( !_serviceNameSet )
	{
		EmaString temp( "Attempt to getServiceName() while it is NOT set." );
		throwIueException( temp );
	}

	return _serviceName.toString();
}

const EmaBuffer& PostMsgDecoder::getPermissionData() const
{
	if ( !hasPermissionData() )
	{
		EmaString temp( "Attempt to getPermissionData() while it is NOT set." );
		throwIueException( temp );
	}

	_permission.setFromInt( _pRsslMsg->postMsg.permData.data, _pRsslMsg->postMsg.permData.length );

	return _permission.toBuffer();
}

UInt32 PostMsgDecoder::getFilter() const
{
	if ( !hasFilter() )
	{
		EmaString temp( "Attempt to getFilter() while it is NOT set." );
		throwIueException( temp );
	}

	return _pRsslMsg->msgBase.msgKey.filter;
}

const EmaBuffer& PostMsgDecoder::getHeader() const
{
	if ( !hasHeader() )
	{
		EmaString temp( "Attempt to getHeader() while it is NOT set." );
		throwIueException( temp );
	}

	_extHeader.setFromInt( _pRsslMsg->requestMsg.extendedHeader.data, _pRsslMsg->requestMsg.extendedHeader.length );

	return _extHeader.toBuffer();
}

const EmaBuffer& PostMsgDecoder::getHexBuffer() const
{
	_hexBuffer.setFromInt( _pRsslMsg->msgBase.encMsgBuffer.data, _pRsslMsg->msgBase.encMsgBuffer.length );

	return _hexBuffer.toBuffer();
}
