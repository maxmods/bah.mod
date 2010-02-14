' Copyright 2010 Bruce A Henderson
'
' Licensed under the Apache License, Version 2.0 (the "License");
' you may not use this file except in compliance with the License.
' You may obtain a copy of the License at
'
'     http://www.apache.org/licenses/LICENSE-2.0
'
' Unless required by applicable law or agreed to in writing, software
' distributed under the License is distributed on an "AS IS" BASIS,
' WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
' See the License for the specific language governing permissions and
' limitations under the License.
'
SuperStrict

Import BRL.Blitz

?win32
Import "../libiconv.mod/include/*.h"
?

Import "src/*.h"

Import "src/zxing/BarcodeFormat.cpp"
Import "src/zxing/Binarizer.cpp"
Import "src/zxing/BinaryBitmap.cpp"
Import "src/zxing/common/Array.cpp"
Import "src/zxing/common/BitArray.cpp"
Import "src/zxing/common/BitMatrix.cpp"
Import "src/zxing/common/BitSource.cpp"
Import "src/zxing/common/Counted.cpp"
Import "src/zxing/common/DecoderResult.cpp"
Import "src/zxing/common/DetectorResult.cpp"
Import "src/zxing/common/EdgeDetector.cpp"
Import "src/zxing/common/GlobalHistogramBinarizer.cpp"
Import "src/zxing/common/GridSampler.cpp"
Import "src/zxing/common/IllegalArgumentException.cpp"
Import "src/zxing/common/LocalBlockBinarizer.cpp"
Import "src/zxing/common/PerspectiveTransform.cpp"
Import "src/zxing/common/reedsolomon/GF256.cpp"
Import "src/zxing/common/reedsolomon/GF256Poly.cpp"
Import "src/zxing/common/reedsolomon/ReedSolomonDecoder.cpp"
Import "src/zxing/common/reedsolomon/ReedSolomonException.cpp"
Import "src/zxing/common/Str.cpp"
Import "src/zxing/Exception.cpp"
Import "src/zxing/LuminanceSource.cpp"
Import "src/zxing/MultiFormatReader.cpp"
Import "src/zxing/oned/Code128Reader.cpp"
Import "src/zxing/oned/Code39Reader.cpp"
Import "src/zxing/oned/EAN13Reader.cpp"
Import "src/zxing/oned/EAN8Reader.cpp"
Import "src/zxing/oned/ITFReader.cpp"
Import "src/zxing/oned/MultiFormatOneDReader.cpp"
Import "src/zxing/oned/MultiFormatUPCEANReader.cpp"
Import "src/zxing/oned/OneDReader.cpp"
Import "src/zxing/oned/OneDResultPoint.cpp"
Import "src/zxing/oned/UPCAReader.cpp"
Import "src/zxing/oned/UPCEANReader.cpp"
Import "src/zxing/oned/UPCEReader.cpp"
Import "src/zxing/qrcode/decoder/BitMatrixParser.cpp"
Import "src/zxing/qrcode/decoder/DataBlock.cpp"
Import "src/zxing/qrcode/decoder/DataMask.cpp"
Import "src/zxing/qrcode/decoder/DecodedBitStreamParser.cpp"
Import "src/zxing/qrcode/decoder/Decoder.cpp"
Import "src/zxing/qrcode/decoder/Mode.cpp"
Import "src/zxing/qrcode/detector/AlignmentPattern.cpp"
Import "src/zxing/qrcode/detector/AlignmentPatternFinder.cpp"
Import "src/zxing/qrcode/detector/Detector.cpp"
Import "src/zxing/qrcode/detector/FinderPattern.cpp"
Import "src/zxing/qrcode/detector/FinderPatternFinder.cpp"
Import "src/zxing/qrcode/detector/FinderPatternInfo.cpp"
Import "src/zxing/qrcode/detector/QREdgeDetector.cpp"
Import "src/zxing/qrcode/ErrorCorrectionLevel.cpp"
Import "src/zxing/qrcode/FormatInformation.cpp"
Import "src/zxing/qrcode/QRCodeReader.cpp"
Import "src/zxing/qrcode/Version.cpp"
Import "src/zxing/Reader.cpp"
Import "src/zxing/ReaderException.cpp"
Import "src/zxing/Result.cpp"
Import "src/zxing/ResultPoint.cpp"

Import "*.h"
Import "glue.cpp"

