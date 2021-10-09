﻿#include "DarkSkin.h"

namespace vl
{
	namespace presentation
	{
		namespace user_resource
		{
			using namespace collections;
			using namespace stream;
			using namespace controls;

			class DarkSkinResourceReader
			{
			public:
				static const vint parserBufferLength = 1705; // 4391 bytes before compressing
				static const vint parserBufferBlock = 1024;
				static const vint parserBufferRemain = 681;
				static const vint parserBufferRows = 2;
				static const char* parserBuffer[2];

				static void ReadToStream(vl::stream::MemoryStream& stream)
				{
					DecompressStream(parserBuffer, true, parserBufferRows, parserBufferBlock, parserBufferRemain, stream);
				}
			};

			const char* DarkSkinResourceReader::parserBuffer[] = {
				"\x27\x11\x00\x00\xA1\x06\x00\x00\x4A\x00\x01\x82\x80\x00\x81\x80\x1E\x29\x65\x73\x2F\x35\x32\x3B\x31\x33\x26\x65\x74\x21\x24\x31\x32\x88\x10\x27\x61\x6D\x25\x3D\x12\x12\x10\x10\x2B\x65\x72\x33\x29\x3F\x36\x34\x8F\x18\x2E\x30\x22\x1E\x1C\x1C\x21\x32\x38\x65\x6E\x24\x31\x93\x31\x35\x85\x17\x2C\xAF\x08\x8A\x8C\x86\x84\x88\x89\x14\x92\xBE\x05\x03\x82\x80\x05\xA2\x01\xC7\x81\x89\x2E\x33\x3A\x88\x37\x0E\x8D\x08\xA0\x02\xAB\xA5\xA7\xA7\x0E\xD3\x34\x39\x3C\x35\x33\x01\xA9\x5F\x81\x84\x2F\x33\x34\x03\xA8\xB3\x01\xD4\x25\x38\x34\x3F\x07\xA8\xB7\x01\xC3\x2C\x21\x33\x3B\x38\x8C\x8D\x08\xE3\x2F\x32\x34\x33\xA8\x01\xB0\x45\xCA\x81\xC2\x84\x21\x32\x39\x35\x53\x6B\x29\x2E\x3E\xBE\xBE\x05\xA9\x06\x10\xC2\x82\x25\x3C\x38\x3B\x92\x8D\x99\xC3\xC8\xA2\xC3\xB3\x31\x3A\x1A\xEE\x34\x3E\xB1\x03\xAA\x01\xA9\x10\x28\xD3\xD4\xB5\x34\x37\xC5\x32\x9E\xA0\xF1\x94\x3E\x5B\xA9\xDA\x83\x9E\xAC\x83\x25\x94\x3B\xD8\x9C\x1E\x5A\xDC\x89\x8C\x91\xE5\xAC\x10\x37\x19\x9B\xA2\x03\x1E\x96\x31\x31\x3A\x6C\x74\x2D\x0F\x26\x39\x92\x39\x34\x33\xAB\xBC\x1A\xBC\x33\xBE\x1F\x11\x46\x61\xE2\xFC\x1F\x14\xEE\xEE\xF2\xC5\xE5\x04\xFB\xA6\xE0\xE6\x8C\x8E\x23\x53\x2D\xC5\x33\x34\x38\x92\x37\xD3\xD5\xF2\x37\xE9\xEC\x96\xF2\x39\xDF\x62\x73\x79\x79\x6F\x1D\x70\x80\x0F\x62\x2B\x1F\x7F\x77\x30\x30\x07\x0D\x10\x43\x03\x14\x79\x82\x0B\xA8\x7C\x7A\x7A\x16\x82\x73\x1D\x4E\xB9\x7E\x58\x59\x51\x1F\x8C\x19\x1A\x53\x9A\x65\x80\x03\x87\x03\x5F\x8D\x13\x88\x63\x26\x8A\x65\x64\x48\x70\x80\x02\x14\x69\x22\x12\x18\x66\x2E\x8D\x62\x8F\x40\x32\x93\x13\x1A\x1C\x05\xAC\x19\x8D\x63\x80\x43\x40\x60\x15\x61\x22\x13\x92\x5F\x07\x13\x5D\x91\x5A\x6B\x74\x12\x13\x1B\x78\x0B\x93\x55\x02\x57\x82\x44\x17\x1B\x05\x8E\x57\x74\x1C\x56\xB1\x82\x65\x40\x57\x0B\x64\x1B\x1B\x77\x21\x95\x50\x03\x53\x6C\x91\x43\x11\x61\x23\x17\x11\x4C\xBB\x6E\x16\x19\x1A\x67\x03\x95\x68\x52\x45\x82\x40\x63\x10\x70\x20\x93\x54\x51\x7C\x93\x55\x55\x51\x87\x80\xA0\x1F\x6E\x6D\x30\x12\x1D\x42\x73\x25\x14\x18\x05\x53\x54\xA2\x5A\x61\x88\x4A\x6E\x1B\x4E\x0B\x4D\x45\x1A\x0B\x63\x01\xAD\x9D\x40\x28\x82\x4F\x87\x5D\x65\x02\x63\x41\x67\x98\x89\x6B\x63\xA8\x54\x46\xA2\x42\x13\x7B\x6D\x11\x18\x1B\x4C\x63\x1C\x19\x1D\x33\x69\x68\x52\xAF\x00\x2E\x0E\x0B\x0B\xC2\x84\xB3\xB0\x9C\x63\x15\x19\x12\x0B\x68\x30\xA1\x07\x54\xCF\x81\x48\x42\x19\x5D\x75\x77\x7D\xAE\xBB\xB3\x49\x0F\x54\xDB\x81\x45\xB3\xB7\xC3\x93\xB5\xB6\x7D\x25\x6F\x04\x15\x1E\x30\x6E\x43\x1F\x8F\x69\x30\x17\x66\x48\x2F\x07\x15\x1D\x1A\xE2\xB4\x74\xBA\x1B\x50\x3B\xA7\x1B\x62\xCC\xB0\xAE\x9B\x9A\x02\x53\x1D\xA6\x43\x46\x05\x83\x4E\x1C\x3B\x82\x4D\x8F\xC0\x65\x3E\x5B\x03\x54\x0F\xC1\x40\x17\xA3\x7A\x4D\xA9\x19\x57\x7D\x53\x5B\x9C\x88\x6F\x5D\xC1\x5F\x5C\x75\x68\xA9\x5F\x5E\x64\x04\x1E\xC5\x51\x70\x40\x04\x47\x61\x73\x1A\xAA\x0E\x0E\x54\x28\x15\x1A\x46\x18\x13\x55\xCD\x40\x2B\xEB\x1D\xCB\x62\x2F\xD4\x66\x65\x49\x6A\x4D\xA3\x5D\x6F\x1A\x13\x55\xD0\xCE\x98\xBB\xCE\x1B\xCB\x43\x32\xC3\x1B\x1A\x53\xB8\x11\xD0\x1C\x43\xE5\x1D\x07\x54\x57\xC8\xDC\xCA\xCB\x3A\x12\x13\x44\x49\x3E\xD7\x6E\x1B\xD4\x55\xDF\x03\x56\xD9\x5A\xFA\xCC\xD5\x6C\xA1\x74\x1C\x11\x92\xAD\x64\xD5\x45\x19\x21\x13\x55\xDD\xDA\x4A\xEF\xCC\xDB\x6C\x6A\x6C\x51\xD6\xDC\xBD\x60\x03\x56\xE0\x78\xEB\xD4\x5A\x6C\xA2\x56\x16\x4F\x1D\x21\x40\xE5\x1B\xD1\x45\x50\xE9\xCD\xDE\x3A\x0C\xA7\x8F\xDF\x33\xD4\xD3\xDC\x09\x53\x5C\xE5\xE0\xCF\x3A\x05\x18\x1C\x1C\x61\x32\x4B\x63\x19\x2E\x7A\x52\x1D\x45\x7B\x4E\xE4\xE1\x51\xAF\xD3\xEC\xD5\x12\xBD\x6D\x12\x11\x9C\x6B\x27\x10\x91\x1D\x32\x4E\xE6\x0B\x54\xBF\xDF\xEB\xD2\x0E\x22\x8E\x5A\xE1\x19\x77\x3B\x6B\xAD\x1B\x6E\x08\x15\x1B\x44\x8D\xD9\xE5\xD4\xE4\x03\x52\xE9\xD0\xD7\xC5\xF4\x17\xF3\x1D\x8E\xD4\xF2\x42\xF5\x5B\xE0\xED\xDE\xDB\x23\xB4\x1D\xF6\xD1\x02\x72\xE2\xFB\xA3\x65\x1B\xFE\xE0\x07\x53\x70\xF2\xF2\xDE\x73\xE1\xD0\xD2\xF4\x73\xF2\xF5\x52\xFE\xEA\xC3\xF5\x61\x6F\x50\x09\x0F\xD3\x51\x72\x69\xB9\x5E\x79\x20\xE0\x6A\x6C\xE8\x47\x0B\x76\x38\x18\x75\x80\xBD\x37\x03\x54\x12\x83\x7E\xE2\x70\x6C\x0D\x8E\x79\x69\x51\x1A\x8D\x7F\x97\x53\x08\x0D\x7B\x34\x0F\x67\x74\x0B\x09\x32\x39\x0E\x71\x22\x13\x29\x0A\x95\x84\x46\x1B\x65\x23\x0A\x54\x69\x0C\x1D\x41\x0B\x3E\x34\x4E\x70\x6D\x45\x38\x85\x07\xDD\x65\x47\x1B\x6E\x0E",
				"\x7E\x4D\x2E\x71\x0B\x53\x23\x88\x16\xBD\x85\x24\x70\x4C\x0D\x57\x72\x09\x0E\x22\x00\x48\x0E\x67\x57\x0F\x0F\x95\x32\x6C\xC7\x67\x03\x2A\x57\x86\x8A\x0B\xEE\x09\x89\x25\xB9\x0C\x76\x33\x79\x7B\x0D\xE7\x69\x7D\xEB\x60\x75\x85\x3E\x9D\x89\x48\x27\x88\x7F\xDE\x29\x03\x2A\x6F\x9A\x89\x1A\x8D\x82\x74\xD2\x72\x20\x88\x8E\x77\x6D\x51\x7A\x8B\x86\x34\x88\x8D\x81\x10\x2E\x0D\x1D\x19\x88\x6B\x32\xA1\x7B\x78\x7E\x9C\x89\x03\xF9\x86\x6E\x43\x8A\x80\x91\x47\x8E\x0D\x12\x7A\x2E\x0D\xB7\x62\x09\x6E\x6D\x85\x0F\x08\x53\x2B\x92\x39\xA5\x27\x6D\x72\x07\x0D\x19\x78\x89\x93\x15\x13\x2E\x94\x9E\x95\x92\x61\x22\x9E\x89\x37\x10\x88\x4D\x53\x9F\x67\xD8\x19\x98\x05\xA9\x35\x91\x95\xA0\x82\x94\x0C\xAE\x32\x4A\x2A\xB7\x78\x82\x9A\x9C\x92\x23\x94\x79\x91\x29\x36\x52\x89\xA4\x80\x98\x2A\xC5\x2B\x99\x3E\x91\x96\x98\xB7\x4C\x0D\x2B\xAF\x92\x8B\x64\x9A\x75\x37\xC1\x65\x29\x36\xCE\x9B\x8A\x68\x8A\x8B\x92\x9F\x97\x82\xE3\x37\x95\x28\x72\x9C\x9E\x87\xDE\x8C\x0F\x2E\xB2\x86\x9B\xAA\x59\x8D\x28\xEF\x87\x9C\xD0\x5C\x4C\x0C\x2F\x6D\x5E\x79\xD0\x72\x0E\xE3\x25\x03\x2A\x7E\xA9\x93\x4B\x5D\x54\x0F\x97\x01\x93\x91\x4C\xAB\x85\x28\x07\xB2\x9F\xD8\x74\x96\x9E\x38\x33\x0E\x71\x9E\x65\x29\x44\xBB\x83\x4B\x9F\x6D\x0E\x47\x40\x48\x9A\x2F\xB3\x90\x98\x1C\x83\x22\x87\x66\x83\x78\x12\x18\xA1\x48\x24\x25\x74\x0C\x5D\x2E\x71\x06\x53\x2B\xA5\x0A\xA3\xA7\x47\x41\x47\x7C\x52\x09\x84\x72\x24\x14\x0B\x75\x03\x9A\x9E\x41\xAA\x83\x98\xEC\x65\x0E\x0C\x74\x04\xA6\x46\xB2\x94\x90\xA4\x1C\x81\xA4\x2F\x73\x63\x1B\x3A\x70\x53\xDB\x7A\xA0\x98\x9D\x85\x29\x54\xCE\x97\x0C\x34\x01\xA8\xA6\x43\xB5\x89\x26\xFA\x73\x20\xFE\x48\xAA\x07\x42\xAC\x0E\xFB\x19\x9A\x8F\x81\x3C\x8F\xAB\x5B\x50\x0D\x58\xED\x93\x6E\x8F\x82\x20\xA4\x87\x8F\x66\x15\x61\xA6\xA4\x99\x29\xA1\x93\x2D\xA5\x29\x5E\xBB\x83\xAE\xAC\x98\x81\x93\x07\x80\x00\x4D\xDC\x66\x0B\x9B\xB8\x74\x80\xD7\x94\x6D\x23\x95\x88\x48\x31\x38\x9F\x60\x6C\xBD\x37\x06\x53\x23\xB2\xC6\x89\x4F\xB1\x88\xB5\x69\x05\x53\x2B\xB2\xCB\xA2\x08\x85\x3C\xB5\x81\x13\x50\x36\x3E\xD7\x2E\x0D\x0C\xE3\x6F\x81\x62\xD1\xA3\x21\xA9\x9F\xAB\x0A\xA7\x7D\x2A\x69\xA8\xB1\x4A\xD5\xA5\x6C\x8C\xA2\xB1\x25\x1D\x4E\xA1\xB3\x32\x2F\x73\x20\xB1\x7F\xA8\x41\xE0\x86\x77\x8D\x8E\x76\x03\x53\x28\xB9\x6E\x82\x9B\x9C\xCB\x19\xB3\x6E\xFE\x85\x29\x74\xBB\x80\xA1\x7A\x82\xA5\x5D\xCD\xB4\x84\x90\x5C\x74\xAD\xC5\xAA\x60\xAD\x0C\xB7\xBA\x03\xC6\xB9\x93\x38\x85\x2E\xBC\xD4\xA1\xBF\x97\x53\x91\x8B\x58\x9A\xAD\xA3\xB8\xBF\xB9\x5C\xB0\x6A\xBD\x76\x44\xA6\x77\xC0\x7D\xA6\xF8\x75\xBA\xAD\x18\xBD\x94\xA8\x99\x8C\xB6\x40\x2E\xB3\xBF\xEA\x8D\xA5\xB9\xBD\x2F\xA9\x62\x95\xB5\x28\x06\xE9\xB9\xA0\x5F\x43\x8C\x52\x70\xAF\x90\xA6\x75\x0E\x29\x6F\x0D\x0D\x9D\x02\xA5\x40\x95\xAC\xA2\xBF\xC3\x75\xBA\x7D\x70\x0E\x71\x9B\x45\x2E\xC4\x3B\x85\x4E\x4C\x36\x89\x92\xBF\x00",
				};

			class DarkSkinResourceLoaderPlugin : public Object, public IGuiPlugin
			{
			public:

				GUI_PLUGIN_NAME(GacGen_DarkSkinResourceLoader)
				{
					GUI_PLUGIN_DEPEND(GacUI_Res_Resource);
					GUI_PLUGIN_DEPEND(GacUI_Res_TypeResolvers);
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
					GUI_PLUGIN_DEPEND(GacUI_Instance_Reflection);
					GUI_PLUGIN_DEPEND(GacUI_Compiler_WorkflowTypeResolvers);
#endif
				}

				void Load()override
				{
					List<GuiResourceError> errors;
					MemoryStream resourceStream;
					DarkSkinResourceReader::ReadToStream(resourceStream);
					resourceStream.SeekFromBegin(0);
					GetResourceManager()->LoadResourceOrPending(resourceStream, GuiResourceUsage::InstanceClass);
				}

				void Unload()override
				{
				}
			};
			GUI_REGISTER_PLUGIN(DarkSkinResourceLoaderPlugin)
		}
	}
}
