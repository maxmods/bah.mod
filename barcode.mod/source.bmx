' Copyright 2010-2013 Bruce A Henderson
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

Import "../bigint.mod/src/*.h"

Import "src/*.h"

Import "src/zxing/aztec/AztecDetectorResult.cpp"
Import "src/zxing/aztec/AztecReader.cpp"
Import "src/zxing/aztec/decoder/Decoder.cpp"
Import "src/zxing/aztec/detector/Detector.cpp"
Import "src/zxing/BarcodeFormat.cpp"
Import "src/zxing/Binarizer.cpp"
Import "src/zxing/BinaryBitmap.cpp"
Import "src/zxing/ChecksumException.cpp"
Import "src/zxing/common/BitArray.cpp"
Import "src/zxing/common/BitArrayIO.cpp"
Import "src/zxing/common/BitMatrix.cpp"
Import "src/zxing/common/BitSource.cpp"
Import "src/zxing/common/CharacterSetECI.cpp"
Import "src/zxing/common/DecoderResult.cpp"
Import "src/zxing/common/detector/MonochromeRectangleDetector.cpp"
Import "src/zxing/common/detector/WhiteRectangleDetector.cpp"
Import "src/zxing/common/DetectorResult.cpp"
Import "src/zxing/common/GlobalHistogramBinarizer.cpp"
Import "src/zxing/common/GreyscaleLuminanceSource.cpp"
Import "src/zxing/common/GreyscaleRotatedLuminanceSource.cpp"
Import "src/zxing/common/GridSampler.cpp"
Import "src/zxing/common/HybridBinarizer.cpp"
Import "src/zxing/common/IllegalArgumentException.cpp"
Import "src/zxing/common/PerspectiveTransform.cpp"
Import "src/zxing/common/reedsolomon/GenericGF.cpp"
Import "src/zxing/common/reedsolomon/GenericGFPoly.cpp"
Import "src/zxing/common/reedsolomon/ReedSolomonDecoder.cpp"
Import "src/zxing/common/reedsolomon/ReedSolomonException.cpp"
Import "src/zxing/common/Str.cpp"
Import "src/zxing/common/StringUtils.cpp"
Import "src/zxing/datamatrix/DataMatrixReader.cpp"
Import "src/zxing/datamatrix/decoder/BitMatrixParser.cpp"
Import "src/zxing/datamatrix/decoder/DataBlock.cpp"
Import "src/zxing/datamatrix/decoder/DecodedBitStreamParser.cpp"
Import "src/zxing/datamatrix/decoder/Decoder.cpp"
Import "src/zxing/datamatrix/detector/CornerPoint.cpp"
Import "src/zxing/datamatrix/detector/Detector.cpp"
Import "src/zxing/datamatrix/detector/DetectorException.cpp"
Import "src/zxing/datamatrix/Version.cpp"
Import "src/zxing/DecodeHints.cpp"
Import "src/zxing/Exception.cpp"
Import "src/zxing/FormatException.cpp"
Import "src/zxing/InvertedLuminanceSource.cpp"
Import "src/zxing/LuminanceSource.cpp"
Import "src/zxing/multi/ByQuadrantReader.cpp"
Import "src/zxing/multi/GenericMultipleBarcodeReader.cpp"
Import "src/zxing/multi/MultipleBarcodeReader.cpp"
Import "src/zxing/multi/qrcode/detector/MultiDetector.cpp"
Import "src/zxing/multi/qrcode/detector/MultiFinderPatternFinder.cpp"
Import "src/zxing/multi/qrcode/QRCodeMultiReader.cpp"
Import "src/zxing/MultiFormatReader.cpp"
Import "src/zxing/oned/CodaBarReader.cpp"
Import "src/zxing/oned/Code128Reader.cpp"
Import "src/zxing/oned/Code39Reader.cpp"
Import "src/zxing/oned/Code93Reader.cpp"
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
Import "src/zxing/pdf417/decoder/BitMatrixParser.cpp"
Import "src/zxing/pdf417/decoder/DecodedBitStreamParser.cpp"
Import "src/zxing/pdf417/decoder/Decoder.cpp"
Import "src/zxing/pdf417/decoder/ec/ErrorCorrection.cpp"
Import "src/zxing/pdf417/decoder/ec/ModulusGF.cpp"
Import "src/zxing/pdf417/decoder/ec/ModulusPoly.cpp"
Import "src/zxing/pdf417/detector/Detector.cpp"
Import "src/zxing/pdf417/detector/LinesSampler.cpp"
Import "src/zxing/pdf417/PDF417Reader.cpp"
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
Import "src/zxing/qrcode/ErrorCorrectionLevel.cpp"
Import "src/zxing/qrcode/FormatInformation.cpp"
Import "src/zxing/qrcode/QRCodeReader.cpp"
Import "src/zxing/qrcode/Version.cpp"
Import "src/zxing/Reader.cpp"
Import "src/zxing/Result.cpp"
Import "src/zxing/ResultIO.cpp"
Import "src/zxing/ResultPoint.cpp"
Import "src/zxing/ResultPointCallback.cpp"

Import "*.h"
Import "glue.cpp"

