include $(SUBDIR)../config.mak

NAME    = bvfilter

HEADERS = bvfilter.h                                                    \
          version.h                                                     \

OBJS    = bvfilter.o allfilters.o bus.o


# Windows resource file
SLIBOBJS-$(HAVE_GNU_WINDRES)                 += avfilterres.o

SKIPHEADERS-$(CONFIG_LIBVIDSTAB)             += vidstabutils.h
SKIPHEADERS-$(CONFIG_OPENCL)                 += opencl_internal.h deshake_opencl_kernel.h unsharp_opencl_kernel.h

OBJS-$(HAVE_THREADS)                         += pthread.o
OBJS-$(CONFIG_SHARED)                        += log2_tab.o

TOOLS     = graph2dot
TESTPROGS = drawutils filtfmts formats

TOOLS-$(CONFIG_LIBZMQ) += zmqsend

clean::
	$(RM) $(CLEANSUFFIXES:%=libavfilter/libmpcodecs/%)
