#include "MWTPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        MWTPKContainer::MWTPKContainer(std::ifstream &stream, uint32_t containerSize) :
                Container(stream),
                m_containerSize(containerSize)
        {
        }

        std::shared_ptr<TPK> MWTPKContainer::Get()
        {
            this->m_tpk = new EAGLEye::Data::TexturePack();
            this->ReadChunks(m_containerSize);

            return std::make_shared<TPK>(*m_tpk);
        }

        size_t MWTPKContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                size_t bytesRead = 0;

//                printf("    TPK Chunk #%02d: 0x%08x\n", i + 1, id);

                switch (id)
                {
                    case MW_TPK:
                    case MW_TPK_TEXTURE_DATA_ROOT:
                    {
                        bytesRead += ReadChunks(size);
                        break;
                    }
                    case MW_TPK_PART_INFO:
                    {
                        m_stream.ignore(4);
                        bytesRead += 4;

                        char name[0x1C];
                        bytesRead += readGenericArray(m_stream, name, sizeof(name));

                        char path[0x40];
                        bytesRead += readGenericArray(m_stream, path, sizeof(path));

                        int32_t hash;
                        bytesRead += readGeneric(m_stream, hash);

                        m_stream.ignore(24);
                        bytesRead += 24;

                        m_tpk->name = std::string(name);
                        m_tpk->tpkPath = std::string(path);
                        m_tpk->hash = hash;

                        printf("TPK: %s [%s] (0x%08x)\n", name, path, hash);

                        break;
                    }
                    case MW_TPK_PART_HASHES:
                    {
                        size_t numHashes = size / 8;

                        printf("Hashes: %zu\n", numHashes);

                        for (int j = 0; j < numHashes; j++)
                        {
                            int hash;
                            bytesRead += readGeneric(m_stream, hash);
                            m_stream.ignore(4);
                            bytesRead += 4;
                        }

                        break;
                    }
                    default:
//                        printf("    * Passing unhandled TPK chunk\n");
                        break;
                }

                assert(bytesRead <= size);

                totalBytesRead += size - bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += bytesRead;
            }

            return totalBytesRead;
        }
    }
}