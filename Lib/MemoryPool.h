#ifndef __MINLIB_MEMORY_POOL__
#define __MINLIB_MEMORY_POOL__


#pragma once
#include <Windows.h>
#include <new>
/*---------------------------------------------------------------
procademy MemoryPool.

�޸� Ǯ Ŭ���� (������Ʈ Ǯ)
Ư�� ����Ÿ(����ü,Ŭ����,����)�� ������ �Ҵ� �� ��������.

- ����.

procademy::CMemoryPool<DATA> MemPool(0, FALSE);
DATA *pData = MemPool.Alloc();

pData ���

MemPool.Free(pData);
----------------------------------------------------------------*/
namespace MinLib
{
	template <class DATA>
	class MemoryPool
	{
	private:
		// �� �� �տ� ���� ��� ����ü.
		struct BLOCK_NODE
		{
			DATA		data		= {};
			BLOCK_NODE* nextBlock	= { nullptr };
			char		endCode		= { 0x77 };
		};

		// ABA ������ �ذ��ϱ� ���� unique���� ������ 16����Ʈ ���� ����ü
		struct ALIGNAS16_BLOCK
		{
			BLOCK_NODE*	topNode = { nullptr };
			INT64		unique	= { 0 };
		};

		alignas(16) ALIGNAS16_BLOCK topNodeBlock_	= {};				// 16����Ʈ ���ĵ� Top Node
		BLOCK_NODE* nullNode_						= { nullptr };		// Null Node
		int		top_								= { 0 };			// Top Index
		int		totalBlockCount_					= { 0 };			// �� ��ü ��
		int		useBlockCount_						= { 0 };			// ������� �� ��
		bool	placementNewFlag_					= { false };		// placementNew ��� ����
	public:
		// ������
		MemoryPool(int blockNum = 0, bool placementNew = false);
		// �Ҹ���
		~MemoryPool();

		// Data �Ҵ�
		DATA* Alloc();
		// Data ����
		bool Free(DATA* pData);
		// Data �� ��ü ��
		int	GetTotalBlockCount();
		// ������� Data �� ��
		int	GetUseBlockCount();
		// Free �� Data �� ����
		bool Clearance(int blockNum = 0);
	};
	/*----------------------------------------------------------*/
	// MemoryPool::MemoryPool (public)
	// ���� : ������
	// ���� : (int) �ʱ� �� ����, (bool) DATA������ ȣ�� ����.
	// ���� :
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
		// �ϳ��ϳ� ���� �����
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
		// �ѹ��� �� �Ҵ��ϰ� ��������
		// ���� �Ҹ��ڿ��� delete�� ���ܹ߻�
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
	// ���� : �ı���
	// ���� : 
	// ���� :
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline MemoryPool<DATA>::~MemoryPool()
	{
		// ����ڰ� ��ȯ���� �ʴ� ���� �������� �ʴ´�.
		BLOCK_NODE* tempNode;
		while (1)
		{
			tempNode = topNodeBlock_.topNode->nextBlock;
			if (!placementNewFlag_)
			{
				//((DATA *)&(_topNodeBlock._topNode->data))->~DATA();
			}
			// ���⼭ DATA�� �Ҹ��ڸ� ȣ���ϰ� �ִ� ���̷���
			delete topNodeBlock_.topNode;
			if (tempNode == nullptr)
				break;
			topNodeBlock_.topNode = tempNode;
		}
	}

	/*----------------------------------------------------------*/
	// MemoryPool::Alloc (public)
	// ���� : DATA �� �ϳ��� �Ҵ�޴´�.
	// ���� : 
	// ���� : (DATA *) ����Ÿ �� ������
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline DATA* MemoryPool<DATA>::Alloc()
	{
		// ���θ��� ��ȯ
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
		// topNode���� ������ ������ ��ȯ
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
	// ���� : ������̴� ���� �����Ѵ�
	// ���� : (DATA *) �� ������
	// ���� : (bool) ���� ���� ����
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline bool MemoryPool<DATA>::Free(DATA* data)
	{
		// ���ÿ� ���� , endCodeȮ��
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
	// ���� : ���� Ȯ�� �� �� ������ ��´�. (�޸�Ǯ ������ ��ü ����)
	// ���� : 
	// ���� : (int) �޸� Ǯ ���� ��ü ����
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline int MemoryPool<DATA>::GetTotalBlockCount()
	{
		return totalBlockCount_;
	}

	/*----------------------------------------------------------*/
	// MemoryPool::GetUseBlockCount (public)
	// ���� : ���� ������� �� ������ ��´�
	// ���� : 
	// ���� : (int) ������� �� ����
	/*----------------*////////////////////////*----------------*/
	template<class DATA>
	inline int MemoryPool<DATA>::GetUseBlockCount()
	{
		return useBlockCount_;
	}

	/*----------------------------------------------------------*/
	// MemoryPool::Clearance (public)
	// ���� : �̻������ ���� �����Ѵ�
	// ���� : (int) ����, 0�̸� ������ ����.
	// ���� : (bool) ��������.
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
	// ���� : MemoryPool TLS ����
	//////////////////////////////////////////////////////////////

	constexpr int CHUNK_SIZE = 500;							// Chunk �� DATA �� ��
	constexpr LONG64 CHUNK_ENDCODE = 0x7777777777777777;	// Chunk ������ EndCode

	template <class DATA>
	class MemoryPoolTLS
	{
	public:
		class Chunk;
		//////////////////////////////////////////////////
		// DATA_BLOCK :
		// ���� : DATA�� �����ϴ� ��
		//////////////////////////////////////////////////
		struct DATA_BLOCK
		{
			DATA	data	= {};						// Data ����
			Chunk*	chunk	= { nullptr };				// �ڽ��� ������ Chunk Pointer
			LONG64	endCode	= { CHUNK_ENDCODE };		// ������ Chunk EndCode
		};

		//////////////////////////////////////////////////
		// Chunk :
		// ���� : MemoryPoolTLS ���� ����ϴ� DATA ���
		//////////////////////////////////////////////////
		class Chunk
		{
			friend class MemoryPool<Chunk>;
			friend class MemoryPoolTLS<DATA>;

		public:
			// ������
			Chunk();
			// DATA ��� �Ҵ�
			DATA* Alloc();
			// DATA Free
			bool Free(DATA_BLOCK* block);
			// DATA Free
			bool Free(DATA* data);

		private:
			void Initialize();

		private:
			DATA_BLOCK		dataBlock_[CHUNK_SIZE]		= {};			// DataBlock ����
			short			useSize_					= { 0 };		// ������� ���� (Thread Safe�ؾ���)
			short			freeSize_					= { 0 };		// �̻������ ���� (Thread Safe�ؾ���)
			MemoryPoolTLS*	memoryPoolTLSPointer_		= { nullptr };	// MemoryPoolTLS Pointer
		};

		friend class Chunk;

	public:
		// ������
		MemoryPoolTLS(bool placementNewFlag = false);
		// �Ҹ���
		~MemoryPoolTLS();

		// Data �� �Ҵ�
		DATA* Alloc();
		// Data �� ����
		bool Free(DATA* block);
		// ��ü Chunk ��
		int GetTotalBlockCount();
		// ������� Chunk ��
		int GetUseBlockCount();

	protected:
		// �� Chunk ����
		void NewChunk(Chunk** chunk);

	private:
		INT					allocCount_				= { 0 };		// �Ҵ� ���� ��
		DWORD				tlsIndex_				= { 0 };		// TLS Index
		bool				placementNewFlag_		= { false };	// placementNew ��� ����
		MemoryPool<Chunk>	memoryPool_				= {};			// Chunk�� �����ϴ� MemoryPool

	};

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::NewChunk (protected)
	// ���� : ���ο� Chunk ����, MemoryPool<Chunk>�κ��� �Ҵ� �޴´�
	// ���� : (Chunk**) �Ҵ���� �� Chunk �������� ������
	// ���� :
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
	// ���� : ������
	// ���� : (bool) placementNew ��� ����
	// ���� :
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline MemoryPoolTLS<DATA>::MemoryPoolTLS(bool placementNewFlag)
		: placementNewFlag_(placementNewFlag)
	{
		tlsIndex_ = TlsAlloc();
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::~MemoryPoolTLS (public)
	// ���� : �Ҹ���
	// ���� : 
	// ���� :
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline MemoryPoolTLS<DATA>::~MemoryPoolTLS()
	{
		// �ڿ� ����?
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::Alloc (public)
	// ���� : Data �� 1�� �Ҵ�, Chunk�� Alloc�� ȣ����
	// ���� : 
	// ���� : Data �� ������
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline DATA* MemoryPoolTLS<DATA>::Alloc()
	{
		Chunk* chunk = (Chunk*)TlsGetValue(tlsIndex_);
		// ������ TLS ����
		if (chunk == nullptr)
			NewChunk(&chunk);

		// ������ ûũ�϶� , �����δ� �׷����� ����
		//if (chunk->_useSize == CHUNK_SIZE)
		//	NewChunk(&chunk);

		// ���� �޸� Alloc
		DATA* block = chunk->Alloc();
		if (placementNewFlag_)
			new (block) DATA;

		// ���������� ������ �� ûũ�� ��ü
		if (chunk->useSize_ == CHUNK_SIZE)
			NewChunk(&chunk);

		InterlockedIncrement((LONG*)& allocCount_);

		return block;
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS::Free (public)
	// ���� : Data ���� Chunk�� �ݳ�, Chunk�� Free�� ȣ����
	// ���� : (Data*) Data �� ������
	// ���� : 
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
	// ���� : ������� Chunk ��
	// ���� : 
	// ���� : (int) Chunk ��
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline int MemoryPoolTLS<DATA>::GetUseBlockCount()
	{
		return memoryPool_.GetUseBlockCount();
	}

	/*----------------------------------------------------------*/
	// MemoryPool::GetTotalBlockCount (public)
	// ���� : ��ü Chunk ��
	// ���� : 
	// ���� : (int) Chunk ��
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
	// ���� : Chunk ������
	// ���� : 
	// ���� :
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
	// ���� : Data �� 1�� �Ҵ�
	// ���� : 
	// ���� : (DATA*) Data �� ������
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline DATA* MemoryPoolTLS<DATA>::Chunk::Alloc()
	{
		return &dataBlock_[useSize_++].data;
	}

	/*----------------------------------------------------------*/
	// MemoryPoolTLS<DATA>::Chunk::Free (public)
	// ���� : Data���� Free ��
	// ���� : 
	// ���� : (bool) Free ���� ����
	/*----------------*////////////////////////*----------------*/
	template <class DATA>
	inline bool MemoryPoolTLS<DATA>::Chunk::Free(DATA_BLOCK* block)
	{
		// end�ڵ带 üũ�� ����
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
	// ���� : Data���� Free ��, ���� ��������� ����
	// ���� : (DATA* data) Data �� ������
	// ���� : (bool) Free ���� ����
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