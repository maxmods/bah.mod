' Copyright (c) 2016 Bruce A Henderson
'
'
' This software is provided 'as-is', without any express or implied
' warranty. In no event will the authors be held liable for any damages
' arising from the use of this software.
'
' Permission is granted to anyone to use this software for any purpose,
' including commercial applications, and to alter it and redistribute it
' freely, subject to the following restrictions:
'
'    1. The origin of this software must not be misrepresented; you must not
'    claim that you wrote the original software. If you use this software
'    in a product, an acknowledgment in the product documentation would be
'    appreciated but is not required.
'
'    2. Altered source versions must be plainly marked as such, and must not be
'    misrepresented as being the original software.
'
'    3. This notice may not be removed or altered from any source
'    distribution.
'
SuperStrict


Import "Embroidermodder/libembroidery/*.h"

Import "Embroidermodder/libembroidery/compound-file.c"
Import "Embroidermodder/libembroidery/compound-file-difat.c"
Import "Embroidermodder/libembroidery/compound-file-directory.c"
Import "Embroidermodder/libembroidery/compound-file-fat.c"
Import "Embroidermodder/libembroidery/compound-file-header.c"
Import "Embroidermodder/libembroidery/emb-arc.c"
Import "Embroidermodder/libembroidery/emb-circle.c"
Import "Embroidermodder/libembroidery/emb-compress.c"
Import "Embroidermodder/libembroidery/emb-color.c"
Import "Embroidermodder/libembroidery/emb-ellipse.c"
Import "Embroidermodder/libembroidery/emb-file.c"
Import "Embroidermodder/libembroidery/emb-flag.c"
Import "Embroidermodder/libembroidery/emb-format.c"
Import "Embroidermodder/libembroidery/emb-hash.c"
Import "Embroidermodder/libembroidery/emb-hoop.c"
Import "Embroidermodder/libembroidery/emb-layer.c"
Import "Embroidermodder/libembroidery/emb-line.c"
Import "Embroidermodder/libembroidery/emb-logging.c"
Import "Embroidermodder/libembroidery/emb-path.c"
Import "Embroidermodder/libembroidery/emb-pattern.c"
Import "Embroidermodder/libembroidery/emb-point.c"
Import "Embroidermodder/libembroidery/emb-polygon.c"
Import "Embroidermodder/libembroidery/emb-polyline.c"
Import "Embroidermodder/libembroidery/emb-reader-writer.c"
Import "Embroidermodder/libembroidery/emb-rect.c"
Import "Embroidermodder/libembroidery/emb-satin-line.c"
Import "Embroidermodder/libembroidery/emb-settings.c"
Import "Embroidermodder/libembroidery/emb-spline.c"
Import "Embroidermodder/libembroidery/emb-stitch.c"
Import "Embroidermodder/libembroidery/emb-thread.c"
Import "Embroidermodder/libembroidery/emb-time.c"
Import "Embroidermodder/libembroidery/emb-vector.c"
Import "Embroidermodder/libembroidery/hashtable.c"
Import "Embroidermodder/libembroidery/helpers-binary.c"
Import "Embroidermodder/libembroidery/helpers-misc.c"
Import "Embroidermodder/libembroidery/thread-color.c"
Import "Embroidermodder/libembroidery/format-10o.c"
Import "Embroidermodder/libembroidery/format-100.c"
Import "Embroidermodder/libembroidery/format-art.c"
Import "Embroidermodder/libembroidery/format-bmc.c"
Import "Embroidermodder/libembroidery/format-bro.c"
Import "Embroidermodder/libembroidery/format-cnd.c"
Import "Embroidermodder/libembroidery/format-col.c"
Import "Embroidermodder/libembroidery/format-csd.c"
Import "Embroidermodder/libembroidery/format-csv.c"
Import "Embroidermodder/libembroidery/format-dat.c"
Import "Embroidermodder/libembroidery/format-dem.c"
Import "Embroidermodder/libembroidery/format-dsb.c"
Import "Embroidermodder/libembroidery/format-dst.c"
Import "Embroidermodder/libembroidery/format-dsz.c"
Import "Embroidermodder/libembroidery/format-dxf.c"
Import "Embroidermodder/libembroidery/format-edr.c"
Import "Embroidermodder/libembroidery/format-emd.c"
Import "Embroidermodder/libembroidery/format-exp.c"
Import "Embroidermodder/libembroidery/format-exy.c"
Import "Embroidermodder/libembroidery/format-eys.c"
Import "Embroidermodder/libembroidery/format-fxy.c"
Import "Embroidermodder/libembroidery/format-gc.c"
Import "Embroidermodder/libembroidery/format-gnc.c"
Import "Embroidermodder/libembroidery/format-gt.c"
Import "Embroidermodder/libembroidery/format-hus.c"
Import "Embroidermodder/libembroidery/format-inb.c"
Import "Embroidermodder/libembroidery/format-inf.c"
Import "Embroidermodder/libembroidery/format-jef.c"
Import "Embroidermodder/libembroidery/format-ksm.c"
Import "Embroidermodder/libembroidery/format-max.c"
Import "Embroidermodder/libembroidery/format-mit.c"
Import "Embroidermodder/libembroidery/format-new.c"
Import "Embroidermodder/libembroidery/format-ofm.c"
Import "Embroidermodder/libembroidery/format-pcd.c"
Import "Embroidermodder/libembroidery/format-pcm.c"
Import "Embroidermodder/libembroidery/format-pcq.c"
Import "Embroidermodder/libembroidery/format-pcs.c"
Import "Embroidermodder/libembroidery/format-pec.c"
Import "Embroidermodder/libembroidery/format-pel.c"
Import "Embroidermodder/libembroidery/format-pem.c"
Import "Embroidermodder/libembroidery/format-pes.c"
Import "Embroidermodder/libembroidery/format-phb.c"
Import "Embroidermodder/libembroidery/format-phc.c"
Import "Embroidermodder/libembroidery/format-plt.c"
Import "Embroidermodder/libembroidery/format-rgb.c"
Import "Embroidermodder/libembroidery/format-sew.c"
Import "Embroidermodder/libembroidery/format-shv.c"
Import "Embroidermodder/libembroidery/format-sst.c"
Import "Embroidermodder/libembroidery/format-stx.c"
Import "Embroidermodder/libembroidery/format-svg.c"
Import "Embroidermodder/libembroidery/format-t09.c"
Import "Embroidermodder/libembroidery/format-tap.c"
Import "Embroidermodder/libembroidery/format-thr.c"
Import "Embroidermodder/libembroidery/format-txt.c"
Import "Embroidermodder/libembroidery/format-u00.c"
Import "Embroidermodder/libembroidery/format-u01.c"
Import "Embroidermodder/libembroidery/format-vip.c"
Import "Embroidermodder/libembroidery/format-vp3.c"
Import "Embroidermodder/libembroidery/format-xxx.c"
Import "Embroidermodder/libembroidery/format-zsk.c"
Import "Embroidermodder/libembroidery/geom-arc.c"
Import "Embroidermodder/libembroidery/geom-circle.c"
Import "Embroidermodder/libembroidery/geom-line.c"

Import "glue.c"

