#ifndef __MINLIB_MEMORY_POOL__
#define __MINLIB_MEMORY_POOL__


#pragma once
#include <Windows.h>
#include <new>
/*---------------------------------------------------------------
procademy MemoryPool.

메모리 풀 클래스 (오브젝트 풀)
특정 데이타(구조체,클래스,변수)를 일정량 할당 후 나눠쓴다.

- 사용법.

procademy::CMemoryPool<DATA> MemPool(0, FALSE);
DATA *pData = MemPool.Alloc();

pData 사용

MemPool.Free(pData);
----------------------------------------------------------------*/
namespace MinLib
{
	template <class DATA>
	class MemoryPool
	{
	private:
		// 각 블럭 앞에 사용될 노드 구조체.
		struct BLOCK_NODE
		{
			DATA		data		= {};
			BLOCK_NODE* nextBlock	= { nullptr };
			char		endCode		= { 0x77 };
		};

		struct BYTE16TOP
		{
			BLOCK_NODE*		topNode;
			INT64			unique;
		};

		alignas(16) BYTE16TOP topNodeBlock_;
		BLOCK_NODE* nullNode_;
		int		top_				= { 0 };
		int		totalBlockCount_	= { 0 };
		int		useBlockCount_		= { 0 };
		bool	placementNewFlag_	= { false };
	public:
		MemoryPool(int blockNum = 0, bool placementNew = false);
		~MemoryPool();

		DATA* Alloc();
		bool Free(DATA* pData);
		int	GetTotalBlockCount();
		int	GetUseBlockCount();
		bool Clearance(int blockNum = 0);
	};
	//////////////////////////////////////////////////////////////////////////
	// 생성자, 파괴자.
	// Parameters:	(int) 초기 블럭 개수.
	//				(bool) 생성자 호출 여부.
	// Return:
	//////////////////////////////////////////////////////////////////////////
	template<class DATA>
	inline MemoryPool<DATA>::MemoryPool(int blockNum, bool placementNew)
	{
		top_ = blockNum - 1;
		totalBlockCount_ = blockNum;
		useBlockCount_ = 0;
		topNodeBlock_.unique = 0;
		placementNewFlag_ = placementNew;
		nullNode_ = new BLOCK_NODE;
		/*---------------*/
		// 하나하나 새로 만들기
		topNodeBlock_.topNode = nullNode_;
		for (int i = 0; i < blockNum; ++i)
		{
			BLOCK_NODE* newNode = new BLOCK_NODE;
			if (!placementNewFlag_)
			{
				DATA* data;
				data = (DATA*)& newNode->data;
				new (data) DATA;
			}
			newNode->nextBlock = topNodeBlock_.topNode;
			newNode->endCode = 0x77;
			topNodeBlock_.topNode = newNode;
		}
		/*---------------*/

		/*---------------*/
		// 한번에 쭉 할당하고 나눠쓰기
		// 현재 소멸자에서 delete시 예외발생
		//_topNode = new BLOCK_NODE[blockNum];
		//for (int i = 0; i < blockNum; ++i)
		//{
		//	if (!_placementNewFlag)
		//	{
		//		DATA * data; 
		//		data = (DATA *)&_topNode[i].data;
		//		new (data) DATA;
		//	}
		//	if(i == blockNum - 1)
		//		_topNode[i].nextBlock = _nullNode;
		//	else
		//		_topNode[i].nextBlock = &_topNode[i + 1];

		//	_topNode[i].endCode = 0x77;
		//}
		/*---------------*/
	}
	// 사용자가 반환하지 않는 블럭은 해제하지 않는다.
	template<class DATA>
	inline MemoryPool<DATA>::~MemoryPool()
	{
		BLOCK_NODE* tempNode;
		while (1)
		{
			tempNode = topNodeBlock_.topNode->nextBlock;
			if (!placementNewFlag_)
			{
				//((DATA *)&(_topNodeBlock._topNode->data))->~DATA();
			}
			// 여기서 DATA의 소멸자를 호출하고 있다 왜이럴까
			delete topNodeBlock_.topNode;
			if (tempNode == nullptr)
				break;
			topNodeBlock_.topNode = tempNode;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 블럭 하나를 할당받는다.
	// Parameters: 없음.
	// Return: (DATA *) 데이타 블럭 포인터.
	//////////////////////////////////////////////////////////////////////////
	template<class DATA>
	inline DATA* MemoryPool<DATA>::Alloc()
	{
		// 새로만들어서 반환
	AllocStart:
		if (topNodeBlock_.topNode == nullNode_)
		{
			BLOCK_NODE* newNode = new BLOCK_NODE;
			DATA* allocData = &(newNode->data);
			if (placementNewFlag_)
				new(allocData) DATA;
			InterlockedIncrement((LONG*)& totalBlockCount_);
			InterlockedIncrement((LONG*)& useBlockCount_);
			InterlockedIncrement64((LONG64*)& topNodeBlock_.unique);
			return allocData;
		}
		// topNode에서 데이터 포인터 반환
		else
		{
			alignas(16) BYTE16TOP popNode;
			//alignas(16) BLOCK_NODE * popNode[2];
			BLOCK_NODE* newTop;
			do
			{
				popNode.unique = topNodeBlock_.unique;
				popNode.topNode = topNodeBlock_.topNode;
				if (popNode.topNode == nullNode_)
					goto AllocStart;
				newTop = popNode.topNode->nextBlock;
			} while (!InterlockedCompareExchange128((LONG64*)& topNodeBlock_, (LONG64)popNode.unique + 1, (LONG64)newTop, (LONG64*)& popNode));

			DATA* allocData = &(popNode.topNode->data);
			if (placementNewFlag_)
				new(allocData) DATA;
			//InterlockedDecrement((LONG *)&_top);
			InterlockedIncrement((LONG*)& useBlockCount_);
			return allocData;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 사용중이던 블럭을 해제한다.
	// Parameters: (DATA *) 블럭 포인터.
	// Return: (BOOL) TRUE, FALSE.
	//////////////////////////////////////////////////////////////////////////
	template<class DATA>
	inline bool MemoryPool<DATA>::Free(DATA* data)
	{
		// 스택에 삽입 , endCode확인
		//BLOCK_NODE * node = (BLOCK_NODE *)(((char *)data) - sizeof(void *));
		BLOCK_NODE* node = (BLOCK_NODE*)data;
		if (node->endCode != 0x77)
			return false;
		if (placementNewFlag_)
			data->~DATA();
		__declspec(align(16)) BLOCK_NODE* snapTop[2];
		do
		{
			snapTop[0] = topNodeBlock_.topNode;
			snapTop[1] = (BLOCK_NODE*)topNodeBlock_.unique;
			node->nextBlock = snapTop[0];
		} while (!InterlockedCompareExchange128((LONG64*)& topNodeBlock_.topNode, (LONG64)(snapTop[1] + 1), (LONG64)node, (LONG64*)snapTop));
		//InterlockedIncrement((LONG *)&_top);
		InterlockedDecrement((LONG*)& useBlockCount_);
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 현재 확보 된 블럭 개수를 얻는다. (메모리풀 내부의 전체 개수)
	// Parameters: 없음.
	// Return: (int) 메모리 풀 내부 전체 개수
	//////////////////////////////////////////////////////////////////////////
	template<class DATA>
	inline int MemoryPool<DATA>::GetTotalBlockCount()
	{
		return totalBlockCount_;
	}

	//////////////////////////////////////////////////////////////////////////
	// 현재 사용중인 블럭 개수를 얻는다.
	// Parameters: 없음.
	// Return: (int) 사용중인 블럭 개수.
	//////////////////////////////////////////////////////////////////////////
	template<class DATA>
	inline int MemoryPool<DATA>::GetUseBlockCount()
	{
		return useBlockCount_;
	}

	//////////////////////////////////////////////////////////////////////////
	// 미사용중인 블럭을 해제한다.
	// Parameters: (int) 갯수, 0이면 현재의 절반.
	// Return: (bool) 성공여부.
	//////////////////////////////////////////////////////////////////////////
	template<class DATA>
	inline bool MemoryPool<DATA>::Clearance(int blockNum)
	{
		if (blockNum == 0)
			blockNum = topNodeBlock_._top / 2;
		if (topNodeBlock_._top + 1 < blockNum)
			return false;

		for (int i = 0; i < blockNum; ++i)
		{
			BLOCK_NODE* tempNode;
			tempNode = topNodeBlock_.topNode->nextBlock;
			if (!placementNewFlag_)
				((DATA*) & (topNodeBlock_.topNode->data))->~DATA();
			delete topNodeBlock_.topNode;
			topNodeBlock_.topNode = tempNode;
		}
		totalBlockCount_ -= blockNum;
		topNodeBlock_._top -= blockNum;
		return true;
	}


	////////////////////////////////////////////////////////////////////


	//#define CHUNK_SIZE 200
	constexpr int CHUNK_SIZE = 500;
	constexpr LONG64 CHUNK_ENDCODE = 0x7777777777777777;

	template <class DATA>
	class MemoryPoolTLS
	{
	public:
		class Chunk;
		struct DATABLOCK
		{
			DATA data;
			Chunk* chunk;
			LONG64 endCode;
			DATABLOCK();
		};

		class Chunk
		{
			DATABLOCK _dataBlock[CHUNK_SIZE];
			short _useSize;
			short _freeSize;
			MemoryPoolTLS* _TLSPointer;
		public:
			Chunk();
			void Init();
			DATA* Alloc();
			bool Free(DATABLOCK* block);
			bool Free(DATA* data);

			friend class MemoryPool<Chunk>;
			friend class MemoryPoolTLS<DATA>;
		};


	private:
		DWORD	_tlsIndex;
		bool	_placementNewFlag;
		MemoryPool<Chunk> _memoryPool;

	protected:
		void NewChunk(Chunk** chunk);

	public:
		INT _allocCount;
		MemoryPoolTLS(bool placementNewFlag = false);
		~MemoryPoolTLS();
		DATA* Alloc();
		bool Free(DATA* block);
		int GetUseBlockCount();
		int GetTotalBlockCount();

		friend class Chunk;
	};

	////////////////////////////////////////////////////////////////////

	template<class DATA>
	inline void MemoryPoolTLS<DATA>::NewChunk(Chunk** chunk)
	{
		Chunk* newChunk = _memoryPool.Alloc();
		newChunk->Init();
		newChunk->_TLSPointer = this;
		TlsSetValue(_tlsIndex, newChunk);
		*chunk = newChunk;
	}

	template <class DATA>
	inline MemoryPoolTLS<DATA>::MemoryPoolTLS(bool placementNewFlag)
	{
		_allocCount = 0;
		_placementNewFlag = placementNewFlag;
		_tlsIndex = TlsAlloc();
	}

	template <class DATA>
	inline MemoryPoolTLS<DATA>::~MemoryPoolTLS()
	{

	}

	template <class DATA>
	inline DATA* MemoryPoolTLS<DATA>::Alloc()
	{
		Chunk* chunk = (Chunk*)TlsGetValue(_tlsIndex);
		// 없으면 TLS 세팅
		if (chunk == nullptr)
			NewChunk(&chunk);

		// 소진된 청크일때 , 실제로는 그럴일이 없음
		//if (chunk->_useSize == CHUNK_SIZE)
		//	NewChunk(&chunk);

		// 실제 메모리 Alloc
		DATA* block = chunk->Alloc();
		if (_placementNewFlag)
		{
			new (block) DATA;
		}

		// 마지막으로 썼으면 새 청크로 교체
		if (chunk->_useSize == CHUNK_SIZE)
			NewChunk(&chunk);

		InterlockedIncrement((LONG*)& _allocCount);

		return block;
	}

	template <class DATA>
	inline bool MemoryPoolTLS<DATA>::Free(DATA* block)
	{
		MemoryPoolTLS<DATA>::DATABLOCK* chunkBlock = (MemoryPoolTLS<DATA>::DATABLOCK*)block;
		if (chunkBlock->endCode != CHUNK_ENDCODE)
			return false;
		InterlockedDecrement((LONG*)& _allocCount);
		return chunkBlock->chunk->Free(chunkBlock);
	}

	template <class DATA>
	inline int MemoryPoolTLS<DATA>::GetUseBlockCount()
	{
		return _memoryPool.GetUseBlockCount();
	}

	template <class DATA>
	inline int MemoryPoolTLS<DATA>::GetTotalBlockCount()
	{
		return _memoryPool.GetTotalBlockCount();
	}

	//Chunk

	template <class DATA>
	inline MemoryPoolTLS<DATA>::Chunk::Chunk()
	{
		Init();
		for (auto& block : _dataBlock)
		{
			block.chunk = this;
		}
	}

	template<class DATA>
	inline void MemoryPoolTLS<DATA>::Chunk::Init()
	{
		_useSize = 0;
		_freeSize = 0;
	}

	template <class DATA>
	inline DATA* MemoryPoolTLS<DATA>::Chunk::Alloc()
	{
		return &_dataBlock[_useSize++].data;
	}

	template <class DATA>
	inline bool MemoryPoolTLS<DATA>::Chunk::Free(DATABLOCK* block)
	{
		// end코드를 체크한 블럭임
		int freeSize = InterlockedIncrement16(&_freeSize);
		if (_TLSPointer->_placementNewFlag)
		{
			block->data.~DATA();
		}
		if (freeSize == CHUNK_SIZE)
			_TLSPointer->_memoryPool.Free(this);
		return true;
	}

	// 실제로 사용하지는 않음
	template <class DATA>
	inline bool MemoryPoolTLS<DATA>::Chunk::Free(DATA* data)
	{
		DATABLOCK* block = (DATABLOCK*)data;
		if (block->endCode != CHUNK_ENDCODE)
			return false;
		return Free(block);
	}

	template <class DATA>
	inline MemoryPoolTLS<DATA>::DATABLOCK::DATABLOCK()
	{
		endCode = CHUNK_ENDCODE;
	}
}
#endif // !__MINLIB_MEMORY_POOL__