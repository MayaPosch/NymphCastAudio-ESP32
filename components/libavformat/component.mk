CPPFLAGS += __FREERTOS__
CFLAGS += -O0 -Wno-format-truncation -Wno-pointer-sign -Wno-parentheses -Wno-switch -Wno-maybe-uninitialized
COMPONENT_ADD_LDFLAGS += --gc-sections
COMPONENT_ADD_INCLUDEDIRS += .
COMPONENT_OBJS := aacdec.o \
			ac3dec.o \
			aiff.o \
			aiffdec.o \
			allformats.o \
			ape.o \
			apetag.o \
			asf.o \
			asf_tags.o \
			asfcrypt.o \
			asfdec_f.o \
			au.o \
			avformat.o \
			avidec.o \
			avio.o \
			aviobuf.o \
			avlanguage.o \
			demux.o \
			demux_utils.o \
			dovi_isom.o \
			dsfdec.o \
			dump.o \
			dv.o \
			file.o \
			flac_picture.o \
			flacdec.o \
			flvdec.o \
			format.o \
			id3v1.o \
			id3v2.o \
			img2.o \
			isom.o \
			isom_tags.o \
			m4vdec.o \
			matroska.o \
			matroskadec.o \
			metadata.o \
			mov.o \
			mov_chan.o \
			mov_esds.o \
			mp3dec.o \
			mpc.o \
			mpc8.o \
			mux.o \
			mux_utils.o \
			oggdec.o \
			oggparsecelt.o \
			oggparsedirac.o \
			oggparseflac.o \
			oggparseogm.o \
			oggparseopus.o \
			oggparseskeleton.o \
			oggparsespeex.o \
			oggparsetheora.o \
			oggparsevorbis.o \
			oggparsevp8.o \
			options.o \
			os_support.o \
			pcm.o \
			pcmdec.o \
			protocols.o \
			qtpalette.o \
			rawdec.o \
			replaygain.o \
			riff.o \
			riffdec.o \
			rm.o \
			rmdec.o \
			rmsipr.o \
			sdp.o \
			seek.o \
			shortendec.o \
			takdec.o \
			tta.o \
			url.o \
			utils.o \
			version.o \
			vorbiscomment.o \
			wavdec.o \
			wv.o \
			wvdec.o \
			xwma.o
COMPONENT_SRCDIRS := libavformat
COMPONENT_ADD_INCLUDEDIRS := libavformat