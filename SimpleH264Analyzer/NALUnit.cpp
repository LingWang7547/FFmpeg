#include "pch.h"
#include "NALUnit.h"

CNALUnit::CNALUnit(uint8* pSODB, uint32 SODBLength)
{
	this->pSODB = pSODB;
	this->SODBLength = SODBLength;
}

CNALUnit::~CNALUnit()
{
}
