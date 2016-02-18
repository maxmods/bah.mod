' Copyright (c) 2007-2016 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Import "src/*.h"
Import "src/FreeImage/*.h"

Import "src/OpenEXR/*.h"
Import "src/OpenEXR/Imath/*.h"
Import "src/OpenEXR/IlmThread/*.h"
Import "src/OpenEXR/IlmImf/*.h"
Import "src/OpenEXR/Half/*.h"
Import "src/OpenEXR/Iex/*.h"
Import "src/ZLib/*.h"

Import "src/FreeImage/PluginEXR.cpp"

Import "src/OpenEXR/IlmImf/ImfAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfB44Compressor.cpp"
Import "src/OpenEXR/IlmImf/ImfBoxAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfChannelList.cpp"
Import "src/OpenEXR/IlmImf/ImfChannelListAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfChromaticities.cpp"
Import "src/OpenEXR/IlmImf/ImfChromaticitiesAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfCompositeDeepScanLine.cpp"
Import "src/OpenEXR/IlmImf/ImfCompressionAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfCompressor.cpp"
Import "src/OpenEXR/IlmImf/ImfConvert.cpp"
Import "src/OpenEXR/IlmImf/ImfCRgbaFile.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepCompositing.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepFrameBuffer.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepImageStateAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepScanLineInputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepScanLineInputPart.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepScanLineOutputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepScanLineOutputPart.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepTiledInputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepTiledInputPart.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepTiledOutputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfDeepTiledOutputPart.cpp"
Import "src/OpenEXR/IlmImf/ImfDoubleAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfDwaCompressor.cpp"
Import "src/OpenEXR/IlmImf/ImfEnvmap.cpp"
Import "src/OpenEXR/IlmImf/ImfEnvmapAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfFastHuf.cpp"
Import "src/OpenEXR/IlmImf/ImfFloatAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfFloatVectorAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfFrameBuffer.cpp"
Import "src/OpenEXR/IlmImf/ImfFramesPerSecond.cpp"
Import "src/OpenEXR/IlmImf/ImfHeader.cpp"
Import "src/OpenEXR/IlmImf/ImfHuf.cpp"
Import "src/OpenEXR/IlmImf/ImfInputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfInputPart.cpp"
Import "src/OpenEXR/IlmImf/ImfInputPartData.cpp"
Import "src/OpenEXR/IlmImf/ImfIntAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfIO.cpp"
Import "src/OpenEXR/IlmImf/ImfKeyCode.cpp"
Import "src/OpenEXR/IlmImf/ImfKeyCodeAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfGenericInputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfGenericOutputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfLineOrderAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfLut.cpp"
Import "src/OpenEXR/IlmImf/ImfMatrixAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfMisc.cpp"
Import "src/OpenEXR/IlmImf/ImfMultiPartInputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfMultiPartOutputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfOpaqueAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfOutputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfPartType.cpp"
Import "src/OpenEXR/IlmImf/ImfPizCompressor.cpp"
Import "src/OpenEXR/IlmImf/ImfPreviewImage.cpp"
Import "src/OpenEXR/IlmImf/ImfPreviewImageAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfPxr24Compressor.cpp"
Import "src/OpenEXR/IlmImf/ImfRational.cpp"
Import "src/OpenEXR/IlmImf/ImfRationalAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfRgbaFile.cpp"
Import "src/OpenEXR/IlmImf/ImfRgbaYca.cpp"
Import "src/OpenEXR/IlmImf/ImfRle.cpp"
Import "src/OpenEXR/IlmImf/ImfRleCompressor.cpp"
Import "src/OpenEXR/IlmImf/ImfScanLineInputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfStandardAttributes.cpp"
Import "src/OpenEXR/IlmImf/ImfStdIO.cpp"
Import "src/OpenEXR/IlmImf/ImfStringAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfStringVectorAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfSystemSpecific.cpp"
Import "src/OpenEXR/IlmImf/ImfTestFile.cpp"
Import "src/OpenEXR/IlmImf/ImfThreading.cpp"
Import "src/OpenEXR/IlmImf/ImfTileDescriptionAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfTiledInputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfTiledMisc.cpp"
Import "src/OpenEXR/IlmImf/ImfTiledOutputFile.cpp"
Import "src/OpenEXR/IlmImf/ImfTiledRgbaFile.cpp"
Import "src/OpenEXR/IlmImf/ImfTileOffsets.cpp"
Import "src/OpenEXR/IlmImf/ImfTimeCode.cpp"
Import "src/OpenEXR/IlmImf/ImfTimeCodeAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfVecAttribute.cpp"
Import "src/OpenEXR/IlmImf/ImfVersion.cpp"
Import "src/OpenEXR/IlmImf/ImfWav.cpp"
Import "src/OpenEXR/IlmImf/ImfZip.cpp"
Import "src/OpenEXR/IlmImf/ImfZipCompressor.cpp"
Import "src/OpenEXR/Imath/ImathBox.cpp"
Import "src/OpenEXR/Imath/ImathColorAlgo.cpp"
Import "src/OpenEXR/Imath/ImathFun.cpp"
Import "src/OpenEXR/Imath/ImathMatrixAlgo.cpp"
Import "src/OpenEXR/Imath/ImathRandom.cpp"
Import "src/OpenEXR/Imath/ImathShear.cpp"
Import "src/OpenEXR/Imath/ImathVec.cpp"
Import "src/OpenEXR/Iex/IexBaseExc.cpp"
Import "src/OpenEXR/Iex/IexThrowErrnoExc.cpp"
Import "src/OpenEXR/Half/half.cpp"
Import "src/OpenEXR/IlmThread/IlmThread.cpp"
Import "src/OpenEXR/IlmThread/IlmThreadMutex.cpp"
Import "src/OpenEXR/IlmThread/IlmThreadPool.cpp"
Import "src/OpenEXR/IlmThread/IlmThreadSemaphore.cpp"
Import "src/OpenEXR/IlmImf/ImfAcesFile.cpp"
Import "src/OpenEXR/IlmImf/ImfMultiView.cpp"


