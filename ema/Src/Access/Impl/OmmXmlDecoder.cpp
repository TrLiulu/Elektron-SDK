/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2015. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "OmmXmlDecoder.h"
#include "ExceptionTranslator.h"

using namespace thomsonreuters::ema::access;

OmmXmlDecoder::OmmXmlDecoder() :
 _rsslBuffer(),
 _toString(),
 _getString(),
 _getBuffer(),
 _dataCode( Data::BlankEnum ),
 _toStringSet( false )
{
}

OmmXmlDecoder::~OmmXmlDecoder()
{
}

void OmmXmlDecoder::setRsslData( RsslDecodeIterator* dIter, RsslBuffer* )
{
	_toStringSet = false;

	RsslRet retCode = rsslDecodeBuffer( dIter, &_rsslBuffer );

	switch ( retCode )
	{
	case RSSL_RET_BLANK_DATA :
		_dataCode = Data::BlankEnum;
		break;
	case RSSL_RET_SUCCESS :
		_dataCode = Data::NoCodeEnum;
		break;
	case RSSL_RET_INCOMPLETE_DATA :
	default :
		{
			_dataCode = Data::BlankEnum;
			EmaString temp( "Failed to decode OmmXml. Reason: " );
			temp += rsslRetCodeToString( retCode );
			throwIueException( temp );
		}
		break;
	}
}

void OmmXmlDecoder::setRsslData( UInt8 , UInt8 , RsslMsg* , const RsslDataDictionary* )
{
}

void OmmXmlDecoder::setRsslData( UInt8 majVer, UInt8 minVer, RsslBuffer* rsslBuffer,
								const RsslDataDictionary* , void* )
{
	RsslDecodeIterator decodeIterator;
	rsslClearDecodeIterator( &decodeIterator );

	_toStringSet = false;

	RsslRet retCode = rsslSetDecodeIteratorBuffer( &decodeIterator, rsslBuffer );
	if ( RSSL_RET_SUCCESS != retCode )
	{
		_dataCode = Data::BlankEnum;
		return;
	}

	retCode = rsslSetDecodeIteratorRWFVersion( &decodeIterator, majVer, minVer );
	if ( RSSL_RET_SUCCESS != retCode )
	{
		_dataCode = Data::BlankEnum;
		return;
	}

	retCode = rsslDecodeBuffer( &decodeIterator, &_rsslBuffer );

	switch ( retCode )
	{
	case RSSL_RET_BLANK_DATA :
		_dataCode = Data::BlankEnum;
		break;
	case RSSL_RET_SUCCESS :
		_dataCode = Data::NoCodeEnum;
		break;
	case RSSL_RET_INCOMPLETE_DATA :
	default :
		{
			_dataCode = Data::BlankEnum;
			EmaString temp( "Failed to decode OmmXml. Reason: " );
			temp += rsslRetCodeToString( retCode );
			throwIueException( temp );
		}
		break;
	}
}

Data::DataCode OmmXmlDecoder::getCode() const
{
	return _dataCode;
}

const EmaString& OmmXmlDecoder::toString()
{
	if ( !_toStringSet )
	{
		_toStringSet = true;

		if ( _dataCode == Data::BlankEnum )
			_toString.setInt( "(blank data)", 12, true );
		else
			_toString.setInt( _rsslBuffer.data, _rsslBuffer.length, false );
	}

	return _toString.toString();
}

const EmaString& OmmXmlDecoder::getString()
{
	_getString.setInt( _rsslBuffer.data, _rsslBuffer.length, false );

	return _getString.toString();
}

const EmaBuffer& OmmXmlDecoder::getBuffer()
{
	_getBuffer.setFromInt( _rsslBuffer.data, _rsslBuffer.length );
	
	return _getBuffer.toBuffer();
}
