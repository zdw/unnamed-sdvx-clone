#include "stdafx.h"
#include "ImageLoader.hpp"
#include "Image.hpp"
#include "png.h"
#include "jpeglib.h"

class ImageLoader_Impl
{

	ImageLoader_Impl()
	{
	}
	~ImageLoader_Impl()
	{
	}
public:
	bool LoadJPEG(ImageRes* pImage, Buffer& in)
	{
		/* This struct contains the JPEG decompression parameters and pointers to
		* working space (which is allocated as needed by the JPEG library).
		*/
		jpeg_decompress_struct cinfo;
		jpeg_error_mgr jerr;
		cinfo.err = jpeg_std_error(&jerr);

		jpeg_create_decompress(&cinfo);
		jpeg_mem_src(&cinfo, in.data(), (uint32)in.size());
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);
		int row_stride = cinfo.output_width * cinfo.output_components;
		JSAMPARRAY sample = (*cinfo.mem->alloc_sarray)
			((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

		Vector2i size = Vector2i(cinfo.output_width, cinfo.output_height);
		pImage->SetSize(size);
		Colori* pBits = pImage->GetBits();

		size_t pixelSize = cinfo.out_color_components;
		cinfo.out_color_space = JCS_RGB;

		while (cinfo.output_scanline < cinfo.output_height) 
		{
			jpeg_read_scanlines(&cinfo, sample, 1);
			for (size_t i = 0; i < cinfo.output_width; i++)
			{
				memcpy(pBits + i, sample[0] + i * pixelSize, pixelSize);
				pBits[i].w = 0xFF;
			}

			pBits += size.x;
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		return 1;
	}
	bool LoadPNG(ImageRes* pImage, Buffer& in)
	{
		png_image image;
		memset(&image, 0, (sizeof image));
		image.version = PNG_IMAGE_VERSION;

		if (png_image_begin_read_from_memory(&image, in.data(), in.size()) == 0)
			return false;

		image.format = PNG_FORMAT_RGBA;

		pImage->SetSize(Vector2i(image.width, image.height));
		Colori* pBuffer = pImage->GetBits();
		if (!pBuffer)
			return false;

		if ((image.width * image.height * 4) != PNG_IMAGE_SIZE(image))
			return false;

		if (png_image_finish_read(&image, nullptr, pBuffer, 0, nullptr) == 0)
			return false;

		png_image_free(&image);
		return true;
	}
	bool Load(ImageRes* pImage, const String& fullPath)
	{
		File f;
		if(!f.OpenRead(fullPath))
			return false;

		Buffer b(f.GetSize());
		f.Read(b.data(), b.size());
		if(b.size() < 4)
			return false;
		
		if (*(uint32*)b.data() == (uint32&)"�PNG")
			return LoadPNG(pImage, b);
		else // jay-PEG ?
			return LoadJPEG(pImage, b);
	}

	static ImageLoader_Impl& Main()
	{
		static ImageLoader_Impl inst;
		return inst;
	}
};


bool ImageLoader::Load(ImageRes* pImage, const String& fullPath)
{
	return ImageLoader_Impl::Main().Load(pImage, fullPath);
}