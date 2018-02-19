#include "UG2LocationContainer.h"
#include "UG2ChunkyContainer.h"
#include "../../chunk/ChunkIO.h"

namespace EAGLEye
{
    namespace Containers
    {
        void UG2LocationContainer::Get()
        {
            long offset;
            long sectionSize = Chunks::SearchAlignedChunkByType(m_stream, 0x00034110, offset);
            unsigned int numSections = sectionSize / (8 + sizeof(Chunks::StreamInfoStruct));

            printf("Sections Offset:     %lu\n", offset);
            printf("Number of Sections:  %d\n", numSections);

            m_stream.seekg(offset + 8);
            m_stream.ignore(80);

            auto path = boost::filesystem::path(m_path).parent_path();

            path.append(std::string("STREAM") + m_path.filename().string());

            std::cout << path.string() << std::endl;
            auto streamInfo = (Chunks::UG2StreamInfo) malloc(sizeof(Chunks::UG2StreamInfoStruct) * numSections);
            std::ifstream mapStream(path.string(), std::ios::binary);

            for (int i = 0; i < numSections; i++)
            {
                readGeneric(m_stream, streamInfo[i]);

                printf("Section %s @ 0x%08x [%d/%d bytes]\n", streamInfo[i].ModelGroupName, streamInfo[i].MasterStreamChunkOffset,
                       streamInfo[i].Size1, streamInfo[i].Size2);

                {
                    UG2ReadOptions options = {};
                    options.start = streamInfo[i].MasterStreamChunkOffset;
                    options.end = streamInfo[i].MasterStreamChunkOffset + streamInfo[i].Size1;

                    auto *chunkContainer = new UG2ChunkyContainer(mapStream, options);

                    chunkContainer->Get();
                }
            }
        }

        size_t UG2LocationContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}