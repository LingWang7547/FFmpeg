#include "pch.h"
#include "Stream.h"
#include "NALUnit.h"

//���캯��
CStreamFile::CStreamFile(char * fileName)
{
	this->fileName = fileName;
	this->file_info();

	this->inputFile = fopen(fileName, "rb");
	if (NULL == inputFile)
	{
		file_error(0);
	}
	
}

//��������
CStreamFile::~CStreamFile()
{
	if (inputFile != NULL)
	{
		fclose(inputFile);
		inputFile = NULL;
	}
}

int CStreamFile::Parse_h264_bitstream()
{
	int ret = 0;
	uint8 nalType = 0;
	do
	{
		ret = find_nal_prefix();
		//����NAL Unit
		//NAL Header��forbidden_zero_bit(1 bit), nal_reference_bit(2bit), nal_unit_type(5bit)
		if (nalVec.size())
		{
			nalType = nalVec[0] & 0x1F; //ȡ��һ���ֽڵĺ���λ
			std::wcout << "NAL Unit Type: " << nalType << std::endl;
			ebsp_to_sodb();
			CNALUnit nalUnit(&nalVec[1], nalVec.size() - 1);
		}
		
		//
	} while (ret != 0);
	return 0;
}

void CStreamFile::file_info()
{
	if (this->fileName)
	{
		std::cout << "File name: " << fileName << std::endl;
	}
}

void CStreamFile::file_error(int idx)
{
	switch (idx)
	{
	case 0:
		std::cout << "Error: opening input file failed" << std::endl;
		break;
	default:
		break;
	}
}

int CStreamFile::find_nal_prefix()
{
	// 00 00 00 01 x x x x x 00 00 00 01
	uint8 prefix[3] = { 0 };
	uint8 fileByte;
	/*
	���αȽ� [0][1][2] = {0 0 0};  �����ǣ�����һ���ַ��ŵ�[0]��λ�� -> [1][2][0] = {0 0 0} ; �´ηŵ�[1]��λ�ã��Դ�����
	�ҵ�������0֮�󣬻����ж���һ���ַ��Ƿ�Ϊ1�� getc() = 1  -> 00 00 00 01
	�Լ��ж� [0][1][2] = {0 0 1} -> [1][2][0] = {0 0 1} �ȣ������������������ʾ�ҵ��ļ�ͷ
	*/

	nalVec.clear();

	// ��ǵ�ǰ�ļ�ָ��λ���Լ���ǲ��ҵ�״̬
	int pos = 0, getPrefix = 0;
	// ��ȡ�����ֽ�
	for (int idx = 0; idx < 3; idx++)
	{
		prefix[idx] = getc(inputFile);
		nalVec.push_back(prefix[idx]);
	}

	while (!feof(inputFile))
	{
		if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 1))
		{
			// 0x 00 00 01 found
			getPrefix = 1;
			nalVec.pop_back();
			nalVec.pop_back();
			nalVec.pop_back();
			break;
		}
		else if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 0))
		{
			if (1 == getc(inputFile))
			{
				// 0x 00 00 00 01 found
				getPrefix = 2;
				nalVec.pop_back();
				nalVec.pop_back();
				nalVec.pop_back();
				break;
			}
		}
		else
		{
			fileByte = getc(inputFile);
			prefix[(pos++) % 3] = fileByte;
			nalVec.push_back(fileByte);
		}
	}

	return getPrefix;
}

void CStreamFile::ebsp_to_sodb()
{
	//����ֻȥ��03����չλ�ã�����ʵ������ebsp_to_rbsp,��ȥ��trailing list����sodb
	if (nalVec.size() < 3)
	{
		return;
	}
	for (std::vector<uint8>::iterator it = nalVec.begin() + 2; it != nalVec.end();)
	{
		if (*it == 3 && *(it - 1) == 0 && *(it - 2) == 0)
		{
			std::vector<uint8>::iterator tmp = nalVec.erase(it);
			it = tmp;
		}
		else
		{
			it++;
		}
	}
}
