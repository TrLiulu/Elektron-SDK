/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2015. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef __thomsonreuters_ema_access_OmmStateDecoder_h
#define __thomsonreuters_ema_access_OmmStateDecoder_h

#include "Decoder.h"
#include "OmmState.h"
#include "EmaStringInt.h"
#include "EmaBufferInt.h"

namespace thomsonreuters {

namespace ema {

namespace access {

class OmmStateDecoder : public Decoder
{
public :

	OmmStateDecoder();

	virtual ~OmmStateDecoder();

	void setRsslData( UInt8 , UInt8 , RsslMsg* , const RsslDataDictionary* );

	void setRsslData( UInt8 , UInt8 , RsslBuffer* , const RsslDataDictionary* , void* );

	void setRsslData( RsslDecodeIterator* , RsslBuffer* );

	void setRsslData( RsslState* );

	Data::DataCode getCode() const;

	const EmaString& toString();

	OmmState::StreamState getStreamState() const;

	OmmState::DataState getDataState() const;

	UInt16 getStatusCode() const;

	const EmaString& getStatusText();

	const EmaBuffer& getHexBuffer();

private :

	RsslBuffer				_rsslBuffer;

	RsslState				_rsslState;

	EmaString				_toString;

	EmaStringInt			_statusText;

	EmaBufferInt			_hexBuffer;

	Data::DataCode			_dataCode;

	bool					_toStringSet;
};

}

}

}

#endif // __thomsonreuters_ema_access_OmmStateDecoder_h
