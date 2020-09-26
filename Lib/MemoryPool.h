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

		// ABA 문제를 해결하기 위해 unique값을 포함한 16바이트 정렬 구조체
		struct ALIGNAS16_BLOCK
		{
			BLOCK_NODE*	topNode = { nullptr };
			INT64		unique	= { 0 };
		};

		alignas(16) ALIGNAS16_BLOCK topNodeBlock_	= {};				// 16바이트 정렬된 Top Node
		BLOCK_NODE* nullNode_						= { nullptr };		// Null Node
		int		top_								= { 0 };			// Top Index
		int		totalBlockCount_					= { 0 };			// 블럭 전체 수
		int		useBlockCount_						= { 0 };			// 사용중인 블럭 수
		bool	placementNewFlag_					= { false };		// placementNew 사용 여부
	public:
		// 생성자
		MemoryPool(int blockNum = 0, bool placementNew = false);
		// 소멸자
		~MemoryPool();

		// Data 할당
		DATA* Alloc();
		// Data 해제
		bool Free(DATA* pData);
		// Data 블럭 전체 수
		int	GetTotalBlockCount();
		// 사용중인 Data 블럭 수
		int	GetUseBlockCount();
		// Free 된 Data 블럭 정리
		bool Clearance(int blockNum = 0);
	};
	/*----------------------------------------------------------*/
	// MemoryPool::MemoryPool (public)
	// 설명 : 생성자
	// 인자 : (int) 초기 블럭 개수, (bool) DATA생성자 호출 여부.
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
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

	/*----------------------------------------------------------*/
	// MemoryPool::~MemoryPool (public)
	// 설명 : 파괴자
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline MemoryPool<DATA>::~MemoryPool()
	{
		// 사용자가 반환하지 않는 블럭은 해제하지 않는다.
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

	/*----------------------------------------------------------*/
	// MemoryPool::Alloc (public)
	// 설명 : DATA 블럭 하나를 할당받는다.
	// 인자 : 
	// 리턴 : (DATA *) 데이타 블럭 포인터
	/*----------------*////////////////////////*----------------*/
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
			alignas(16) ALIGNAS16_BLOCK popNode;
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

	/*----------------------------------------------------------*/
	// MemoryPool::Free (virtual) (private) (protected) (public)
	// 설명 : 사용중이던 블럭을 해제한다
	// 인자 : (DATA *) 블럭 포인터
	// 리턴 : (bool) 해제 성공 여부
	/*----------------*////////////////////////*----------------*/
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

	/*----------------------------------------------------------*/
	// MemoryPool::GetTotalBlockCount (public)
	// 설명 : 현재 확보 된 블럭 개수를 얻는다. (메모리풀 내부의 전체 개수)
	// 인자 : 
	// 리턴 : (int) 메모리 풀 내부 전체 개수
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline int MemoryPool<DATA>::GetTotalBlockCount()
	{
		return totalBlockCount_;
	}

	/*----------------------------------------------------------*/
	// MemoryPool::GetUseBlockCount (public)
	// 설명 : 현재 사용중인 블럭 개수를 얻는다
	// 인자 : 
	// 리턴 : (int) 사용중인 블럭 개수
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline int MemoryPool<DATA>::GetUseBlockCount()
	{
		return useBlockCount_;
	}

	/*----------------------------------------------------------*/
	// MemoryPool::Clearance (public)
	// 설명 : 미사용중인 블럭을 해제한다
	// 인자 : (int) 갯수, 0이면 현재의 절반.
	// 리턴 : (bool) 성공여부.
	/*----------------*////////////////////////*----------------*/
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

	//////////////////////////////////////////////////////////////
	// MemoryPoolTLS :
	// 설명 : MemoryPool TLS 버전
	//////////////////////////////////////////////////////////////

	constexpr int CHUNK_SIZE = 500;							// Chunk 당 DATA 블럭 수
	constexpr LONG64 CHUNK_ENDCODE = 0x7777777777777777;	// Chunk 검증용 EndCode

	template <class DATA>
	class MemoryPoolTLS
	{
	public:
		class Chunk;
		//////////////////////////////////////////////////
		// DATA_BLOCK :
		// 설명 : DATA를 포함하는 블럭
		//////////////////////////////////////////////////
		struct DATA_BLOCK
		{
			DATA	data	= {};						// Data 영역
			Chunk*	chunk	= { nullptr };				// 자신을 포함한 Chunk Pointer
			LONG64	endCode	= { CHUNK_ENDCODE };		// 검증용 Chunk EndCode
		};

		//////////////////////////////////////////////////
		// Chunk :
		// 설명 : MemoryPoolTLS 에서 사용하는 DATA 덩어리
		//////////////////////////////////////////////////
		class Chunk
		{
			friend class MemoryPool<Chunk>;
			friend class MemoryPoolTLS<DATA>;

		public:
			// 생성자
			Chunk();
			// DATA 블록 할당
			DATA* Alloc();
			// DATA Free
			bool Free(DATA_BLOCK* block);
			// DATA Free
			bool Free(DATA* data);

		private:
			void Initialize();

		private:
			DATA_BLOCK		dataBlock_[CHUNK_SIZE]		= {};			// DataBlock 영역
			short			useSize_					= { 0 };		// 사용중인 개수 (Thread Safe해야함)
			short			freeSize_					= { 0 };		// 미사용중인 개수 (Thread Safe해야함)
			MemoryPoolTLS*	memoryPoolTLSPointer_		= { nullptr };	// MemoryPoolTLS Pointer
		};

		friend class Chunk;

	public:
		// 생성자
		MemoryPoolTLS(bool placementNewFlag = false);
		// 소멸자
		~MemoryPoolTLS();

		// Data 블럭 할당
		DATA* Alloc();
		// Data 블럭 해제
		bool Free(DATA* block);
		// 전체 Chunk 수
		int GetTotalBlockCount();
		// 사용중인 Chunk 수
		int GetUseBlockCount();

	protected:
		// 새 Chunk 생성
		void NewChunk(Chunk** chunk);

	private:
		INT					allocCount_				= { 0 };		// 할당 받은 수
		DWORD				tlsIndex_				= { 0 };		// TLS Index
		bool				placementNewFlag_		= { false };	// placementNew 사용 여부
		MemoryPool<Chunk>	memoryPool_				= {};			// Chunk를 관리하는 MemoryPool

	};

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::NewChunk (protected)
	// 설명 : 새로운 Chunk 생성, MemoryPool<Chunk>로부터 할당 받는다
	// 인자 : (Chunk**) 할당받을 새 Chunk 포인터의 포인터
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline void MemoryPoolTLS<DATA>::NewChunk(Chunk** chunk)
	{
		Chunk* newChunk = memoryPool_.Alloc();
		newChunk->Initialize();
		newChunk->memoryPoolTLSPointer_ = this;
		TlsSetValue(tlsIndex_, newChunk);
		*chunk = newChunk;
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::MemoryPoolTLS (public)
	// 설명 : 생성자
	// 인자 : (bool) placementNew 사용 여부
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline MemoryPoolTLS<DATA>::MemoryPoolTLS(bool placementNewFlag)
		: placementNewFlag_(placementNewFlag)
	{
		tlsIndex_ = TlsAlloc();
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::~MemoryPoolTLS (public)
	// 설명 : 소멸자
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline MemoryPoolTLS<DATA>::~MemoryPoolTLS()
	{
		// 자원 정리?
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::Alloc (public)
	// 설명 : Data 블럭 1개 할당, Chunk의 Alloc을 호출함
	// 인자 : 
	// 리턴 : Data 블럭 포인터
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline DATA* MemoryPoolTLS<DATA>::Alloc()
	{
		Chunk* chunk = (Chunk*)TlsGetValue(tlsIndex_);
		// 없으면 TLS 세팅
		if (chunk == nullptr)
			NewChunk(&chunk);

		// 소진된 청크일때 , 실제로는 그럴일이 없음
		//if (chunk->_useSize == CHUNK_SIZE)
		//	NewChunk(&chunk);

		// 실제 메모리 Alloc
		DATA* block = chunk->Alloc();
		if (placementNewFlag_)
			new (block) DATA;

		// 마지막으로 썼으면 새 청크로 교체
		if (chunk->useSize_ == CHUNK_SIZE)
			NewChunk(&chunk);

		InterlockedIncrement((LONG*)& allocCount_);

		return block;
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::Free (public)
	// 설명 : Data 블럭을 Chunk에 반납, Chunk의 Free를 호출함
	// 인자 : (Data*) Data 블럭 포인터
	// 리턴 : 
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline bool MemoryPoolTLS<DATA>::Free(DATA* block)
	{
		MemoryPoolTLS<DATA>::DATA_BLOCK* chunkBlock = (MemoryPoolTLS<DATA>::DATA_BLOCK*)block;
		if (chunkBlock->endCode != CHUNK_ENDCODE)
			return false;
		InterlockedDecrement((LONG*)& allocCount_);
		return chunkBlock->chunk->Free(chunkBlock);
	}

	/*----------------------------------------------------------*/
	// MemoryPool::GetUseBlockCount (public)
	// 설명 : 사용중인 Chunk 수
	// 인자 : 
	// 리턴 : (int) Chunk 수
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline int MemoryPoolTLS<DATA>::GetUseBlockCount()
	{
		return memoryPool_.GetUseBlockCount();
	}

	/*----------------------------------------------------------*/
	// MemoryPool::GetTotalBlockCount (public)
	// 설명 : 전체 Chunk 수
	// 인자 : 
	// 리턴 : (int) Chunk 수
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline int MemoryPoolTLS<DATA>::GetTotalBlockCount()
	{
		return memoryPool_.GetTotalBlockCount();
	}

	//////////////////////////////////////////////////////////////
	// Chunk
	//////////////////////////////////////////////////////////////
	/*----------------------------------------------------------*/
	// MemoryPoolTLS<DATA>::Chunk::Chunk (public)
	// 설명 : Chunk 생성자
	// 인자 : 
	// 리턴 :
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline MemoryPoolTLS<DATA>::Chunk::Chunk()
	{
		for (auto& block : dataBlock_)
		{
			block.chunk = this;
		}
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS<DATA>::Chunk::Alloc (public)
	// 설명 : Data 블럭 1개 할당
	// 인자 : 
	// 리턴 : (DATA*) Data 블럭 포인터
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline DATA* MemoryPoolTLS<DATA>::Chunk::Alloc()
	{
		return &dataBlock_[useSize_++].data;
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS<DATA>::Chunk::Free (public)
	// 설명 : Data블럭을 Free 함
	// 인자 : 
	// 리턴 : (bool) Free 성공 여부
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline bool MemoryPoolTLS<DATA>::Chunk::Free(DATA_BLOCK* block)
	{
		// end코드를 체크한 블럭임
		int freeSize = InterlockedIncrement16(&freeSize_);
		if (memoryPoolTLSPointer_->placementNewFlag_)
		{
			block->data.~DATA();
		}
		if (freeSize == CHUNK_SIZE)
			memoryPoolTLSPointer_->memoryPool_.Free(this);
		return true;
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS<DATA>::Chunk::Free (public)
	// 설명 : Data블럭을 Free 함, 현재 사용하지는 않음
	// 인자 : (DATA* data) Data 블럭 포인터
	// 리턴 : (bool) Free 성공 여부
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline bool MemoryPoolTLS<DATA>::Chunk::Free(DATA* data)
	{
		DATA_BLOCK* block = (DATA_BLOCK*)data;
		if (block->endCode != CHUNK_ENDCODE)
			return false;
		return Free(block);
	}

	template<class DATA>
	inline void MemoryPoolTLS<DATA>::Chunk::Initialize()
	{
		useSize_ = 0;
		freeSize_ = 0;
	}

}
#endif // !__MINLIB_MEMORY_POOL__