#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 505

#define MGK_BlobErrorUnableToCreateBlob 1
#define MGK_BlobErrorUnableToOpenFile 2
#define MGK_BlobErrorUnableToReadFile 3
#define MGK_BlobErrorUnableToReadToOffset 4
#define MGK_BlobErrorUnableToSeekToOffset 5
#define MGK_BlobErrorUnableToWriteBlob 6
#define MGK_BlobErrorUnrecognizedImageFormat 7
#define MGK_BlobFatalErrorDefault 8
#define MGK_BlobWarningDefault 9
#define MGK_CacheErrorInconsistentPersistentCacheDepth 10
#define MGK_CacheErrorPixelCacheIsNotOpen 11
#define MGK_CacheErrorUnableToAllocateCacheView 12
#define MGK_CacheErrorUnableToCloneCache 13
#define MGK_CacheErrorUnableToExtendCache 14
#define MGK_CacheErrorUnableToGetCacheNexus 15
#define MGK_CacheErrorUnableToGetPixelsFromCache 16
#define MGK_CacheErrorUnableToOpenCache 17
#define MGK_CacheErrorUnableToPeristPixelCache 18
#define MGK_CacheErrorUnableToReadPixelCache 19
#define MGK_CacheErrorUnableToSyncCache 20
#define MGK_CacheFatalErrorDiskAllocationFailed 21
#define MGK_CacheFatalErrorUnableToExtendPixelCache 22
#define MGK_CacheWarningDefault 23
#define MGK_CoderErrorColormapTooLarge 24
#define MGK_CoderErrorColormapTypeNotSupported 25
#define MGK_CoderErrorColorspaceModelIsNotSupported 26
#define MGK_CoderErrorColorTypeNotSupported 27
#define MGK_CoderErrorCompressionNotValid 28
#define MGK_CoderErrorDataEncodingSchemeIsNotSupported 29
#define MGK_CoderErrorDataStorageTypeIsNotSupported 30
#define MGK_CoderErrorDeltaPNGNotSupported 31
#define MGK_CoderErrorEncryptedWPGImageFileNotSupported 32
#define MGK_CoderErrorFractalCompressionNotSupported 33
#define MGK_CoderErrorImageColumnOrRowSizeIsNotSupported 34
#define MGK_CoderErrorImageDoesNotHaveAMatteChannel 35
#define MGK_CoderErrorImageIsNotTiled 36
#define MGK_CoderErrorImageTypeNotSupported 37
#define MGK_CoderErrorIncompatibleSizeOfDouble 38
#define MGK_CoderErrorIrregularChannelGeometryNotSupported 39
#define MGK_CoderErrorJNGCompressionNotSupported 40
#define MGK_CoderErrorJPEGCompressionNotSupported 41
#define MGK_CoderErrorJPEGEmbeddingFailed 42
#define MGK_CoderErrorLocationTypeIsNotSupported 43
#define MGK_CoderErrorMapStorageTypeIsNotSupported 44
#define MGK_CoderErrorMSBByteOrderNotSupported 45
#define MGK_CoderErrorMultidimensionalMatricesAreNotSupported 46
#define MGK_CoderErrorMultipleRecordListNotSupported 47
#define MGK_CoderErrorNo8BIMDataIsAvailable 48
#define MGK_CoderErrorNoAPP1DataIsAvailable 49
#define MGK_CoderErrorNoBitmapOnClipboard 50
#define MGK_CoderErrorNoColorProfileAvailable 51
#define MGK_CoderErrorNoDataReturned 52
#define MGK_CoderErrorNoImageVectorGraphics 53
#define MGK_CoderErrorNoIPTCInfoWasFound 54
#define MGK_CoderErrorNoIPTCProfileAvailable 55
#define MGK_CoderErrorNumberOfImagesIsNotSupported 56
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 57
#define MGK_CoderErrorOnlyLevelZerofilesSupported 58
#define MGK_CoderErrorPNGCompressionNotSupported 59
#define MGK_CoderErrorPNGLibraryTooOld 60
#define MGK_CoderErrorRLECompressionNotSupported 61
#define MGK_CoderErrorSubsamplingRequiresEvenWidth 62
#define MGK_CoderErrorUnableToCopyProfile 63
#define MGK_CoderErrorUnableToCreateADC 64
#define MGK_CoderErrorUnableToCreateBitmap 65
#define MGK_CoderErrorUnableToDecompressImage 66
#define MGK_CoderErrorUnableToInitializeFPXLibrary 67
#define MGK_CoderErrorUnableToOpenBlob 68
#define MGK_CoderErrorUnableToReadAspectRatio 69
#define MGK_CoderErrorUnableToReadCIELABImages 70
#define MGK_CoderErrorUnableToReadSummaryInfo 71
#define MGK_CoderErrorUnableToSetAffineMatrix 72
#define MGK_CoderErrorUnableToSetAspectRatio 73
#define MGK_CoderErrorUnableToSetColorTwist 74
#define MGK_CoderErrorUnableToSetContrast 75
#define MGK_CoderErrorUnableToSetFilteringValue 76
#define MGK_CoderErrorUnableToSetImageComments 77
#define MGK_CoderErrorUnableToSetImageTitle 78
#define MGK_CoderErrorUnableToSetJPEGLevel 79
#define MGK_CoderErrorUnableToSetRegionOfInterest 80
#define MGK_CoderErrorUnableToSetSummaryInfo 81
#define MGK_CoderErrorUnableToTranslateText 82
#define MGK_CoderErrorUnableToWriteMPEGParameters 83
#define MGK_CoderErrorUnableToZipCompressImage 84
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 85
#define MGK_CoderErrorZipCompressionNotSupported 86
#define MGK_CoderFatalErrorDefault 87
#define MGK_CoderWarningLosslessToLossyJPEGConversion 88
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 89
#define MGK_ConfigureErrorRegistryKeyLookupFailed 90
#define MGK_ConfigureErrorStringTokenLengthExceeded 91
#define MGK_ConfigureErrorUnableToAccessConfigureFile 92
#define MGK_ConfigureErrorUnableToAccessFontFile 93
#define MGK_ConfigureErrorUnableToAccessLogFile 94
#define MGK_ConfigureErrorUnableToAccessModuleFile 95
#define MGK_ConfigureFatalErrorDefault 96
#define MGK_ConfigureWarningDefault 97
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 98
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 99
#define MGK_CorruptImageErrorColormapExceeds256Colors 100
#define MGK_CorruptImageErrorCorruptImage 101
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 102
#define MGK_CorruptImageErrorImageTypeNotSupported 103
#define MGK_CorruptImageErrorImproperImageHeader 104
#define MGK_CorruptImageErrorInsufficientImageDataInFile 105
#define MGK_CorruptImageErrorInvalidColormapIndex 106
#define MGK_CorruptImageErrorInvalidFileFormatVersion 107
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 108
#define MGK_CorruptImageErrorMissingImageChannel 109
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 110
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 111
#define MGK_CorruptImageErrorNotEnoughTiles 112
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 113
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 114
#define MGK_CorruptImageErrorTooMuchImageDataInFile 115
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 116
#define MGK_CorruptImageErrorUnableToReadColorProfile 117
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 118
#define MGK_CorruptImageErrorUnableToReadGenericProfile 119
#define MGK_CorruptImageErrorUnableToReadImageData 120
#define MGK_CorruptImageErrorUnableToReadImageHeader 121
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 122
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 123
#define MGK_CorruptImageErrorUnableToReadSubImageData 124
#define MGK_CorruptImageErrorUnableToReadVIDImage 125
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 126
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 127
#define MGK_CorruptImageErrorUnableToUncompressImage 128
#define MGK_CorruptImageErrorUnexpectedEndOfFile 129
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 130
#define MGK_CorruptImageErrorUnknownPatternType 131
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 132
#define MGK_CorruptImageErrorUnrecognizedImageCompression 133
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 134
#define MGK_CorruptImageFatalErrorUnableToPersistKey 135
#define MGK_CorruptImageWarningCompressionNotValid 136
#define MGK_CorruptImageWarningImproperImageHeader 137
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 138
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 139
#define MGK_CorruptImageWarningSkipToSyncByte 140
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 141
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 142
#define MGK_CorruptImageWarningUnrecognizedImageCompression 143
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 144
#define MGK_DelegateErrorDelegateFailed 145
#define MGK_DelegateErrorFailedToAllocateArgumentList 146
#define MGK_DelegateErrorFailedToAllocateGhostscriptInterpreter 147
#define MGK_DelegateErrorFailedToComputeOutputSize 148
#define MGK_DelegateErrorFailedToRenderFile 149
#define MGK_DelegateErrorFailedToScanFile 150
#define MGK_DelegateErrorNoTagFound 151
#define MGK_DelegateErrorPostscriptDelegateFailed 152
#define MGK_DelegateErrorUnableToCreateImage 153
#define MGK_DelegateErrorUnableToCreateImageComponent 154
#define MGK_DelegateErrorUnableToDecodeImageFile 155
#define MGK_DelegateErrorUnableToEncodeImageFile 156
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 157
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 158
#define MGK_DelegateErrorUnableToManageJP2Stream 159
#define MGK_DelegateErrorUnableToWriteSVGFormat 160
#define MGK_DelegateFatalErrorDefault 161
#define MGK_DelegateWarningDefault 162
#define MGK_DrawErrorAlreadyPushingPatternDefinition 163
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 164
#define MGK_DrawErrorUnableToPrint 165
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 166
#define MGK_DrawFatalErrorDefault 167
#define MGK_DrawWarningNotARelativeURL 168
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 169
#define MGK_DrawWarningURLNotFound 170
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 171
#define MGK_FileOpenErrorUnableToOpenFile 172
#define MGK_FileOpenErrorUnableToWriteFile 173
#define MGK_FileOpenFatalErrorDefault 174
#define MGK_FileOpenWarningDefault 175
#define MGK_ImageErrorAngleIsDiscontinuous 176
#define MGK_ImageErrorColorspaceColorProfileMismatch 177
#define MGK_ImageErrorImageColorspaceDiffers 178
#define MGK_ImageErrorImageDifferenceExceedsLimit 179
#define MGK_ImageErrorImageDoesNotContainResolution 180
#define MGK_ImageErrorImageOpacityDiffers 181
#define MGK_ImageErrorImageSequenceIsRequired 182
#define MGK_ImageErrorImageSizeDiffers 183
#define MGK_ImageErrorInvalidColormapIndex 184
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 185
#define MGK_ImageErrorNoImagesWereFound 186
#define MGK_ImageErrorNoImagesWereLoaded 187
#define MGK_ImageErrorNoLocaleImageAttribute 188
#define MGK_ImageErrorTooManyClusters 189
#define MGK_ImageErrorUnableToAppendImage 190
#define MGK_ImageErrorUnableToAssignProfile 191
#define MGK_ImageErrorUnableToAverageImage 192
#define MGK_ImageErrorUnableToCoalesceImage 193
#define MGK_ImageErrorUnableToCompareImages 194
#define MGK_ImageErrorUnableToCreateImageMosaic 195
#define MGK_ImageErrorUnableToCreateStereoImage 196
#define MGK_ImageErrorUnableToDeconstructImageSequence 197
#define MGK_ImageErrorUnableToFlattenImage 198
#define MGK_ImageErrorUnableToGetClipMask 199
#define MGK_ImageErrorUnableToResizeImage 200
#define MGK_ImageErrorUnableToSegmentImage 201
#define MGK_ImageErrorUnableToSetClipMask 202
#define MGK_ImageErrorUnableToShearImage 203
#define MGK_ImageErrorWidthOrHeightExceedsLimit 204
#define MGK_ImageFatalErrorUnableToPersistKey 205
#define MGK_ImageWarningDefault 206
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 207
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 208
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 209
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 210
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 211
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 212
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 213
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 214
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 215
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 216
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 217
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 218
#define MGK_MissingDelegateFatalErrorDefault 219
#define MGK_MissingDelegateWarningDefault 220
#define MGK_ModuleErrorFailedToCloseModule 221
#define MGK_ModuleErrorFailedToFindSymbol 222
#define MGK_ModuleErrorUnableToLoadModule 223
#define MGK_ModuleErrorUnableToRegisterImageFormat 224
#define MGK_ModuleErrorUnrecognizedModule 225
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 226
#define MGK_ModuleWarningDefault 227
#define MGK_OptionErrorBevelWidthIsNegative 228
#define MGK_OptionErrorColorSeparatedImageRequired 229
#define MGK_OptionErrorFrameIsLessThanImageSize 230
#define MGK_OptionErrorGeometryDimensionsAreZero 231
#define MGK_OptionErrorGeometryDoesNotContainImage 232
#define MGK_OptionErrorHaldClutImageDimensionsInvalid 233
#define MGK_OptionErrorImagesAreNotTheSameSize 234
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 235
#define MGK_OptionErrorImageSmallerThanKernelWidth 236
#define MGK_OptionErrorImageSmallerThanRadius 237
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 238
#define MGK_OptionErrorInputImagesAlreadySpecified 239
#define MGK_OptionErrorInvalidSubimageSpecification 240
#define MGK_OptionErrorKernelRadiusIsTooSmall 241
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 242
#define MGK_OptionErrorMatrixIsNotSquare 243
#define MGK_OptionErrorMatrixOrderOutOfRange 244
#define MGK_OptionErrorMissingAnImageFilename 245
#define MGK_OptionErrorMissingArgument 246
#define MGK_OptionErrorMustSpecifyAnImageName 247
#define MGK_OptionErrorMustSpecifyImageSize 248
#define MGK_OptionErrorNoBlobDefined 249
#define MGK_OptionErrorNoImagesDefined 250
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 251
#define MGK_OptionErrorNoProfileNameWasGiven 252
#define MGK_OptionErrorNullBlobArgument 253
#define MGK_OptionErrorReferenceImageRequired 254
#define MGK_OptionErrorReferenceIsNotMyType 255
#define MGK_OptionErrorRequestDidNotReturnAnImage 256
#define MGK_OptionErrorSteganoImageRequired 257
#define MGK_OptionErrorStereoImageRequired 258
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 259
#define MGK_OptionErrorUnableToAddOrRemoveProfile 260
#define MGK_OptionErrorUnableToAverageImageSequence 261
#define MGK_OptionErrorUnableToBlurImage 262
#define MGK_OptionErrorUnableToChopImage 263
#define MGK_OptionErrorUnableToColorMatrixImage 264
#define MGK_OptionErrorUnableToConstituteImage 265
#define MGK_OptionErrorUnableToConvolveImage 266
#define MGK_OptionErrorUnableToEdgeImage 267
#define MGK_OptionErrorUnableToEqualizeImage 268
#define MGK_OptionErrorUnableToFilterImage 269
#define MGK_OptionErrorUnableToFormatImageMetadata 270
#define MGK_OptionErrorUnableToFrameImage 271
#define MGK_OptionErrorUnableToOilPaintImage 272
#define MGK_OptionErrorUnableToPaintImage 273
#define MGK_OptionErrorUnableToRaiseImage 274
#define MGK_OptionErrorUnableToSharpenImage 275
#define MGK_OptionErrorUnableToThresholdImage 276
#define MGK_OptionErrorUnableToWaveImage 277
#define MGK_OptionErrorUnrecognizedAttribute 278
#define MGK_OptionErrorUnrecognizedChannelType 279
#define MGK_OptionErrorUnrecognizedColor 280
#define MGK_OptionErrorUnrecognizedColormapType 281
#define MGK_OptionErrorUnrecognizedColorspace 282
#define MGK_OptionErrorUnrecognizedCommand 283
#define MGK_OptionErrorUnrecognizedComposeOperator 284
#define MGK_OptionErrorUnrecognizedDisposeMethod 285
#define MGK_OptionErrorUnrecognizedElement 286
#define MGK_OptionErrorUnrecognizedEndianType 287
#define MGK_OptionErrorUnrecognizedGravityType 288
#define MGK_OptionErrorUnrecognizedHighlightStyle 289
#define MGK_OptionErrorUnrecognizedImageCompression 290
#define MGK_OptionErrorUnrecognizedImageFilter 291
#define MGK_OptionErrorUnrecognizedImageFormat 292
#define MGK_OptionErrorUnrecognizedImageMode 293
#define MGK_OptionErrorUnrecognizedImageType 294
#define MGK_OptionErrorUnrecognizedIntentType 295
#define MGK_OptionErrorUnrecognizedInterlaceType 296
#define MGK_OptionErrorUnrecognizedListType 297
#define MGK_OptionErrorUnrecognizedMetric 298
#define MGK_OptionErrorUnrecognizedModeType 299
#define MGK_OptionErrorUnrecognizedNoiseType 300
#define MGK_OptionErrorUnrecognizedOperator 301
#define MGK_OptionErrorUnrecognizedOption 302
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 303
#define MGK_OptionErrorUnrecognizedPixelMap 304
#define MGK_OptionErrorUnrecognizedPreviewType 305
#define MGK_OptionErrorUnrecognizedResourceType 306
#define MGK_OptionErrorUnrecognizedType 307
#define MGK_OptionErrorUnrecognizedUnitsType 308
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 309
#define MGK_OptionErrorUnsupportedSamplingFactor 310
#define MGK_OptionErrorUsageError 311
#define MGK_OptionFatalErrorInvalidColorspaceType 312
#define MGK_OptionFatalErrorInvalidEndianType 313
#define MGK_OptionFatalErrorInvalidImageType 314
#define MGK_OptionFatalErrorInvalidInterlaceType 315
#define MGK_OptionFatalErrorMissingAnImageFilename 316
#define MGK_OptionFatalErrorMissingArgument 317
#define MGK_OptionFatalErrorNoImagesWereLoaded 318
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 319
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 320
#define MGK_OptionFatalErrorUnableToOpenXServer 321
#define MGK_OptionFatalErrorUnableToPersistKey 322
#define MGK_OptionFatalErrorUnrecognizedColormapType 323
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 324
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 325
#define MGK_OptionFatalErrorUnrecognizedEndianType 326
#define MGK_OptionFatalErrorUnrecognizedFilterType 327
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 328
#define MGK_OptionFatalErrorUnrecognizedImageType 329
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 330
#define MGK_OptionFatalErrorUnrecognizedOption 331
#define MGK_OptionFatalErrorUnrecognizedResourceType 332
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 333
#define MGK_OptionWarningUnrecognizedColor 334
#define MGK_RegistryErrorImageExpected 335
#define MGK_RegistryErrorImageInfoExpected 336
#define MGK_RegistryErrorStructureSizeMismatch 337
#define MGK_RegistryErrorUnableToGetRegistryID 338
#define MGK_RegistryErrorUnableToLocateImage 339
#define MGK_RegistryErrorUnableToSetRegistry 340
#define MGK_RegistryFatalErrorDefault 341
#define MGK_RegistryWarningDefault 342
#define MGK_ResourceLimitErrorCacheResourcesExhausted 343
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 344
#define MGK_ResourceLimitErrorMemoryAllocationFailed 345
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 346
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 347
#define MGK_ResourceLimitErrorUnableToAddColorProfile 348
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 349
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 350
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 351
#define MGK_ResourceLimitErrorUnableToAllocateColormap 352
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 353
#define MGK_ResourceLimitErrorUnableToAllocateString 354
#define MGK_ResourceLimitErrorUnableToAnnotateImage 355
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 356
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 357
#define MGK_ResourceLimitErrorUnableToCloneImage 358
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 359
#define MGK_ResourceLimitErrorUnableToConstituteImage 360
#define MGK_ResourceLimitErrorUnableToConvertFont 361
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 362
#define MGK_ResourceLimitErrorUnableToCreateColormap 363
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 364
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 365
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 366
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 367
#define MGK_ResourceLimitErrorUnableToCreateXWindow 368
#define MGK_ResourceLimitErrorUnableToCropImage 369
#define MGK_ResourceLimitErrorUnableToDespeckleImage 370
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 371
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 372
#define MGK_ResourceLimitErrorUnableToDitherImage 373
#define MGK_ResourceLimitErrorUnableToDrawOnImage 374
#define MGK_ResourceLimitErrorUnableToEdgeImage 375
#define MGK_ResourceLimitErrorUnableToEmbossImage 376
#define MGK_ResourceLimitErrorUnableToEnhanceImage 377
#define MGK_ResourceLimitErrorUnableToFloodfillImage 378
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 379
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 380
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 381
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 382
#define MGK_ResourceLimitErrorUnableToLevelImage 383
#define MGK_ResourceLimitErrorUnableToMagnifyImage 384
#define MGK_ResourceLimitErrorUnableToManageColor 385
#define MGK_ResourceLimitErrorUnableToMapImage 386
#define MGK_ResourceLimitErrorUnableToMapImageSequence 387
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 388
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 389
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 390
#define MGK_ResourceLimitErrorUnableToNormalizeImage 391
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 392
#define MGK_ResourceLimitErrorUnableToQuantizeImage 393
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 394
#define MGK_ResourceLimitErrorUnableToReadTextChunk 395
#define MGK_ResourceLimitErrorUnableToReadXImage 396
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 397
#define MGK_ResourceLimitErrorUnableToResizeImage 398
#define MGK_ResourceLimitErrorUnableToRotateImage 399
#define MGK_ResourceLimitErrorUnableToSampleImage 400
#define MGK_ResourceLimitErrorUnableToScaleImage 401
#define MGK_ResourceLimitErrorUnableToSelectImage 402
#define MGK_ResourceLimitErrorUnableToSharpenImage 403
#define MGK_ResourceLimitErrorUnableToShaveImage 404
#define MGK_ResourceLimitErrorUnableToShearImage 405
#define MGK_ResourceLimitErrorUnableToSortImageColormap 406
#define MGK_ResourceLimitErrorUnableToThresholdImage 407
#define MGK_ResourceLimitErrorUnableToTransformColorspace 408
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 409
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 410
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 411
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 412
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 413
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 414
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 415
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 416
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 417
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 418
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 419
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 420
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 421
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 422
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 423
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 424
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 425
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 426
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 427
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 428
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 429
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 430
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 431
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 432
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 433
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 434
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 435
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 436
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 437
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 438
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 439
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 440
#define MGK_ResourceLimitFatalErrorUnableToConvertText 441
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 442
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 443
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 444
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 445
#define MGK_ResourceLimitWarningMemoryAllocationFailed 446
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 447
#define MGK_StreamErrorNoStreamHandlerIsDefined 448
#define MGK_StreamErrorPixelCacheIsNotOpen 449
#define MGK_StreamErrorUnableToAcquirePixelStream 450
#define MGK_StreamErrorUnableToSetPixelStream 451
#define MGK_StreamErrorUnableToSyncPixelStream 452
#define MGK_StreamFatalErrorDefault 453
#define MGK_StreamWarningDefault 454
#define MGK_TypeErrorFontSubstitutionRequired 455
#define MGK_TypeErrorUnableToGetTypeMetrics 456
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 457
#define MGK_TypeErrorUnableToReadFont 458
#define MGK_TypeErrorUnrecognizedFontEncoding 459
#define MGK_TypeFatalErrorDefault 460
#define MGK_TypeWarningDefault 461
#define MGK_WandErrorInvalidColormapIndex 462
#define MGK_WandErrorWandAPINotImplemented 463
#define MGK_WandErrorWandContainsNoImageIndexs 464
#define MGK_WandErrorWandContainsNoImages 465
#define MGK_XServerErrorColorIsNotKnownToServer 466
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 467
#define MGK_XServerErrorStandardColormapIsNotInitialized 468
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 469
#define MGK_XServerErrorUnableToCreateBitmap 470
#define MGK_XServerErrorUnableToCreateColormap 471
#define MGK_XServerErrorUnableToCreatePixmap 472
#define MGK_XServerErrorUnableToCreateProperty 473
#define MGK_XServerErrorUnableToCreateStandardColormap 474
#define MGK_XServerErrorUnableToDisplayImageInfo 475
#define MGK_XServerErrorUnableToGetProperty 476
#define MGK_XServerErrorUnableToGetStandardColormap 477
#define MGK_XServerErrorUnableToGetVisual 478
#define MGK_XServerErrorUnableToGrabMouse 479
#define MGK_XServerErrorUnableToLoadFont 480
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 481
#define MGK_XServerErrorUnableToOpenXServer 482
#define MGK_XServerErrorUnableToReadXAttributes 483
#define MGK_XServerErrorUnableToReadXWindowImage 484
#define MGK_XServerErrorUnrecognizedColormapType 485
#define MGK_XServerErrorUnrecognizedGravityType 486
#define MGK_XServerErrorUnrecognizedVisualSpecifier 487
#define MGK_XServerFatalErrorUnableToAllocateXHints 488
#define MGK_XServerFatalErrorUnableToCreateCursor 489
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 490
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 491
#define MGK_XServerFatalErrorUnableToCreateTextProperty 492
#define MGK_XServerFatalErrorUnableToCreateXImage 493
#define MGK_XServerFatalErrorUnableToCreateXPixmap 494
#define MGK_XServerFatalErrorUnableToCreateXWindow 495
#define MGK_XServerFatalErrorUnableToDisplayImage 496
#define MGK_XServerFatalErrorUnableToDitherImage 497
#define MGK_XServerFatalErrorUnableToGetPixelInfo 498
#define MGK_XServerFatalErrorUnableToGetVisual 499
#define MGK_XServerFatalErrorUnableToLoadFont 500
#define MGK_XServerFatalErrorUnableToMakeXWindow 501
#define MGK_XServerFatalErrorUnableToOpenXServer 502
#define MGK_XServerFatalErrorUnableToViewFonts 503
#define MGK_XServerWarningUnableToGetVisual 504
#define MGK_XServerWarningUsingDefaultVisual 505

#endif

#if defined(_INCLUDE_CATEGORYMAP_TABLE_)
typedef struct _CategoryInfo{
  const char *name;
  int offset;
} CategoryInfo;

static const CategoryInfo category_map[] =
  {
    { "Blob", 0 },
    { "Cache", 3 },
    { "Coder", 6 },
    { "Configure", 9 },
    { "Corrupt/Image", 12 },
    { "Delegate", 15 },
    { "Draw", 18 },
    { "File/Open", 21 },
    { "Image", 24 },
    { "Missing/Delegate", 27 },
    { "Module", 30 },
    { "Option", 33 },
    { "Registry", 36 },
    { "Resource/Limit", 39 },
    { "Stream", 42 },
    { "Type", 45 },
    { "Wand", 48 },
    { "XServer", 49 },
    { 0, 51 }
  };
#endif

#if defined(_INCLUDE_SEVERITYMAP_TABLE_)
typedef struct _SeverityInfo{
  const char *name;
  int offset;
  ExceptionType severityid;
} SeverityInfo;

static const SeverityInfo severity_map[] =
  {
    { "Blob/Error", 0, BlobError },
    { "Blob/FatalError", 7, BlobFatalError },
    { "Blob/Warning", 8, BlobWarning },
    { "Cache/Error", 9, CacheError },
    { "Cache/FatalError", 20, CacheFatalError },
    { "Cache/Warning", 22, CacheWarning },
    { "Coder/Error", 23, CoderError },
    { "Coder/FatalError", 86, CoderFatalError },
    { "Coder/Warning", 87, CoderWarning },
    { "Configure/Error", 88, ConfigureError },
    { "Configure/FatalError", 95, ConfigureFatalError },
    { "Configure/Warning", 96, ConfigureWarning },
    { "Corrupt/Image/Error", 97, CorruptImageError },
    { "Corrupt/Image/FatalError", 134, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 135, CorruptImageWarning },
    { "Delegate/Error", 144, DelegateError },
    { "Delegate/FatalError", 160, DelegateFatalError },
    { "Delegate/Warning", 161, DelegateWarning },
    { "Draw/Error", 162, DrawError },
    { "Draw/FatalError", 166, DrawFatalError },
    { "Draw/Warning", 167, DrawWarning },
    { "File/Open/Error", 170, FileOpenError },
    { "File/Open/FatalError", 173, FileOpenFatalError },
    { "File/Open/Warning", 174, FileOpenWarning },
    { "Image/Error", 175, ImageError },
    { "Image/FatalError", 204, ImageFatalError },
    { "Image/Warning", 205, ImageWarning },
    { "Missing/Delegate/Error", 206, MissingDelegateError },
    { "Missing/Delegate/FatalError", 218, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 219, MissingDelegateWarning },
    { "Module/Error", 220, ModuleError },
    { "Module/FatalError", 225, ModuleFatalError },
    { "Module/Warning", 226, ModuleWarning },
    { "Option/Error", 227, OptionError },
    { "Option/FatalError", 311, OptionFatalError },
    { "Option/Warning", 333, OptionWarning },
    { "Registry/Error", 334, RegistryError },
    { "Registry/FatalError", 340, RegistryFatalError },
    { "Registry/Warning", 341, RegistryWarning },
    { "Resource/Limit/Error", 342, ResourceLimitError },
    { "Resource/Limit/FatalError", 408, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 445, ResourceLimitWarning },
    { "Stream/Error", 446, StreamError },
    { "Stream/FatalError", 452, StreamFatalError },
    { "Stream/Warning", 453, StreamWarning },
    { "Type/Error", 454, TypeError },
    { "Type/FatalError", 459, TypeFatalError },
    { "Type/Warning", 460, TypeWarning },
    { "Wand/Error", 461, WandError },
    { "XServer/Error", 465, XServerError },
    { "XServer/FatalError", 487, XServerFatalError },
    { "XServer/Warning", 503, XServerWarning },
    { 0, 505, UndefinedException }
  };
#endif

#if defined(_INCLUDE_TAGMAP_TABLE_)
typedef struct _MessageInfo
{
  const char *name;
  int messageid;
} MessageInfo;

static const MessageInfo message_map[] =
  {
    { "UnableToCreateBlob", 1 },
    { "UnableToOpenFile", 2 },
    { "UnableToReadFile", 3 },
    { "UnableToReadToOffset", 4 },
    { "UnableToSeekToOffset", 5 },
    { "UnableToWriteBlob", 6 },
    { "UnrecognizedImageFormat", 7 },
    { "Default", 8 },
    { "Default", 9 },
    { "InconsistentPersistentCacheDepth", 10 },
    { "PixelCacheIsNotOpen", 11 },
    { "UnableToAllocateCacheView", 12 },
    { "UnableToCloneCache", 13 },
    { "UnableToExtendCache", 14 },
    { "UnableToGetCacheNexus", 15 },
    { "UnableToGetPixelsFromCache", 16 },
    { "UnableToOpenCache", 17 },
    { "UnableToPeristPixelCache", 18 },
    { "UnableToReadPixelCache", 19 },
    { "UnableToSyncCache", 20 },
    { "DiskAllocationFailed", 21 },
    { "UnableToExtendPixelCache", 22 },
    { "Default", 23 },
    { "ColormapTooLarge", 24 },
    { "ColormapTypeNotSupported", 25 },
    { "ColorspaceModelIsNotSupported", 26 },
    { "ColorTypeNotSupported", 27 },
    { "CompressionNotValid", 28 },
    { "DataEncodingSchemeIsNotSupported", 29 },
    { "DataStorageTypeIsNotSupported", 30 },
    { "DeltaPNGNotSupported", 31 },
    { "EncryptedWPGImageFileNotSupported", 32 },
    { "FractalCompressionNotSupported", 33 },
    { "ImageColumnOrRowSizeIsNotSupported", 34 },
    { "ImageDoesNotHaveAMatteChannel", 35 },
    { "ImageIsNotTiled", 36 },
    { "ImageTypeNotSupported", 37 },
    { "IncompatibleSizeOfDouble", 38 },
    { "IrregularChannelGeometryNotSupported", 39 },
    { "JNGCompressionNotSupported", 40 },
    { "JPEGCompressionNotSupported", 41 },
    { "JPEGEmbeddingFailed", 42 },
    { "LocationTypeIsNotSupported", 43 },
    { "MapStorageTypeIsNotSupported", 44 },
    { "MSBByteOrderNotSupported", 45 },
    { "MultidimensionalMatricesAreNotSupported", 46 },
    { "MultipleRecordListNotSupported", 47 },
    { "No8BIMDataIsAvailable", 48 },
    { "NoAPP1DataIsAvailable", 49 },
    { "NoBitmapOnClipboard", 50 },
    { "NoColorProfileAvailable", 51 },
    { "NoDataReturned", 52 },
    { "NoImageVectorGraphics", 53 },
    { "NoIPTCInfoWasFound", 54 },
    { "NoIPTCProfileAvailable", 55 },
    { "NumberOfImagesIsNotSupported", 56 },
    { "OnlyContinuousTonePictureSupported", 57 },
    { "OnlyLevelZerofilesSupported", 58 },
    { "PNGCompressionNotSupported", 59 },
    { "PNGLibraryTooOld", 60 },
    { "RLECompressionNotSupported", 61 },
    { "SubsamplingRequiresEvenWidth", 62 },
    { "UnableToCopyProfile", 63 },
    { "UnableToCreateADC", 64 },
    { "UnableToCreateBitmap", 65 },
    { "UnableToDecompressImage", 66 },
    { "UnableToInitializeFPXLibrary", 67 },
    { "UnableToOpenBlob", 68 },
    { "UnableToReadAspectRatio", 69 },
    { "UnableToReadCIELABImages", 70 },
    { "UnableToReadSummaryInfo", 71 },
    { "UnableToSetAffineMatrix", 72 },
    { "UnableToSetAspectRatio", 73 },
    { "UnableToSetColorTwist", 74 },
    { "UnableToSetContrast", 75 },
    { "UnableToSetFilteringValue", 76 },
    { "UnableToSetImageComments", 77 },
    { "UnableToSetImageTitle", 78 },
    { "UnableToSetJPEGLevel", 79 },
    { "UnableToSetRegionOfInterest", 80 },
    { "UnableToSetSummaryInfo", 81 },
    { "UnableToTranslateText", 82 },
    { "UnableToWriteMPEGParameters", 83 },
    { "UnableToZipCompressImage", 84 },
    { "UnsupportedCellTypeInTheMatrix", 85 },
    { "ZipCompressionNotSupported", 86 },
    { "Default", 87 },
    { "LosslessToLossyJPEGConversion", 88 },
    { "IncludeElementNestedTooDeeply", 89 },
    { "RegistryKeyLookupFailed", 90 },
    { "StringTokenLengthExceeded", 91 },
    { "UnableToAccessConfigureFile", 92 },
    { "UnableToAccessFontFile", 93 },
    { "UnableToAccessLogFile", 94 },
    { "UnableToAccessModuleFile", 95 },
    { "Default", 96 },
    { "Default", 97 },
    { "AnErrorHasOccurredReadingFromFile", 98 },
    { "AnErrorHasOccurredWritingToFile", 99 },
    { "ColormapExceeds256Colors", 100 },
    { "CorruptImage", 101 },
    { "ImageFileDoesNotContainAnyImageData", 102 },
    { "ImageTypeNotSupported", 103 },
    { "ImproperImageHeader", 104 },
    { "InsufficientImageDataInFile", 105 },
    { "InvalidColormapIndex", 106 },
    { "InvalidFileFormatVersion", 107 },
    { "LengthAndFilesizeDoNotMatch", 108 },
    { "MissingImageChannel", 109 },
    { "NegativeOrZeroImageSize", 110 },
    { "NonOS2HeaderSizeError", 111 },
    { "NotEnoughTiles", 112 },
    { "StaticPlanesValueNotEqualToOne", 113 },
    { "SubsamplingRequiresEvenWidth", 114 },
    { "TooMuchImageDataInFile", 115 },
    { "UnableToReadColormapFromDumpFile", 116 },
    { "UnableToReadColorProfile", 117 },
    { "UnableToReadExtensionBlock", 118 },
    { "UnableToReadGenericProfile", 119 },
    { "UnableToReadImageData", 120 },
    { "UnableToReadImageHeader", 121 },
    { "UnableToReadIPTCProfile", 122 },
    { "UnableToReadPixmapFromDumpFile", 123 },
    { "UnableToReadSubImageData", 124 },
    { "UnableToReadVIDImage", 125 },
    { "UnableToReadWindowNameFromDumpFile", 126 },
    { "UnableToRunlengthDecodeImage", 127 },
    { "UnableToUncompressImage", 128 },
    { "UnexpectedEndOfFile", 129 },
    { "UnexpectedSamplingFactor", 130 },
    { "UnknownPatternType", 131 },
    { "UnrecognizedBitsPerPixel", 132 },
    { "UnrecognizedImageCompression", 133 },
    { "UnrecognizedXWDHeader", 134 },
    { "UnableToPersistKey", 135 },
    { "CompressionNotValid", 136 },
    { "ImproperImageHeader", 137 },
    { "NegativeOrZeroImageSize", 138 },
    { "NonOS2HeaderSizeError", 139 },
    { "SkipToSyncByte", 140 },
    { "StaticPlanesValueNotEqualToOne", 141 },
    { "UnrecognizedBitsPerPixel", 142 },
    { "UnrecognizedImageCompression", 143 },
    { "UnrecognizedNumberOfColors", 144 },
    { "DelegateFailed", 145 },
    { "FailedToAllocateArgumentList", 146 },
    { "FailedToAllocateGhostscriptInterpreter", 147 },
    { "FailedToComputeOutputSize", 148 },
    { "FailedToRenderFile", 149 },
    { "FailedToScanFile", 150 },
    { "NoTagFound", 151 },
    { "PostscriptDelegateFailed", 152 },
    { "UnableToCreateImage", 153 },
    { "UnableToCreateImageComponent", 154 },
    { "UnableToDecodeImageFile", 155 },
    { "UnableToEncodeImageFile", 156 },
    { "UnableToInitializeFPXLibrary", 157 },
    { "UnableToInitializeWMFLibrary", 158 },
    { "UnableToManageJP2Stream", 159 },
    { "UnableToWriteSVGFormat", 160 },
    { "Default", 161 },
    { "Default", 162 },
    { "AlreadyPushingPatternDefinition", 163 },
    { "NonconformingDrawingPrimitiveDefinition", 164 },
    { "UnableToPrint", 165 },
    { "UnbalancedGraphicContextPushPop", 166 },
    { "Default", 167 },
    { "NotARelativeURL", 168 },
    { "NotCurrentlyPushingPatternDefinition", 169 },
    { "URLNotFound", 170 },
    { "UnableToCreateTemporaryFile", 171 },
    { "UnableToOpenFile", 172 },
    { "UnableToWriteFile", 173 },
    { "Default", 174 },
    { "Default", 175 },
    { "AngleIsDiscontinuous", 176 },
    { "ColorspaceColorProfileMismatch", 177 },
    { "ImageColorspaceDiffers", 178 },
    { "ImageDifferenceExceedsLimit", 179 },
    { "ImageDoesNotContainResolution", 180 },
    { "ImageOpacityDiffers", 181 },
    { "ImageSequenceIsRequired", 182 },
    { "ImageSizeDiffers", 183 },
    { "InvalidColormapIndex", 184 },
    { "LeftAndRightImageSizesDiffer", 185 },
    { "NoImagesWereFound", 186 },
    { "NoImagesWereLoaded", 187 },
    { "NoLocaleImageAttribute", 188 },
    { "TooManyClusters", 189 },
    { "UnableToAppendImage", 190 },
    { "UnableToAssignProfile", 191 },
    { "UnableToAverageImage", 192 },
    { "UnableToCoalesceImage", 193 },
    { "UnableToCompareImages", 194 },
    { "UnableToCreateImageMosaic", 195 },
    { "UnableToCreateStereoImage", 196 },
    { "UnableToDeconstructImageSequence", 197 },
    { "UnableToFlattenImage", 198 },
    { "UnableToGetClipMask", 199 },
    { "UnableToResizeImage", 200 },
    { "UnableToSegmentImage", 201 },
    { "UnableToSetClipMask", 202 },
    { "UnableToShearImage", 203 },
    { "WidthOrHeightExceedsLimit", 204 },
    { "UnableToPersistKey", 205 },
    { "Default", 206 },
    { "DPSLibraryIsNotAvailable", 207 },
    { "FPXLibraryIsNotAvailable", 208 },
    { "FreeTypeLibraryIsNotAvailable", 209 },
    { "JPEGLibraryIsNotAvailable", 210 },
    { "LCMSLibraryIsNotAvailable", 211 },
    { "LZWEncodingNotEnabled", 212 },
    { "NoDecodeDelegateForThisImageFormat", 213 },
    { "NoEncodeDelegateForThisImageFormat", 214 },
    { "TIFFLibraryIsNotAvailable", 215 },
    { "XMLLibraryIsNotAvailable", 216 },
    { "XWindowLibraryIsNotAvailable", 217 },
    { "ZipLibraryIsNotAvailable", 218 },
    { "Default", 219 },
    { "Default", 220 },
    { "FailedToCloseModule", 221 },
    { "FailedToFindSymbol", 222 },
    { "UnableToLoadModule", 223 },
    { "UnableToRegisterImageFormat", 224 },
    { "UnrecognizedModule", 225 },
    { "UnableToInitializeModuleLoader", 226 },
    { "Default", 227 },
    { "BevelWidthIsNegative", 228 },
    { "ColorSeparatedImageRequired", 229 },
    { "FrameIsLessThanImageSize", 230 },
    { "GeometryDimensionsAreZero", 231 },
    { "GeometryDoesNotContainImage", 232 },
    { "HaldClutImageDimensionsInvalid", 233 },
    { "ImagesAreNotTheSameSize", 234 },
    { "ImageSizeMustExceedBevelWidth", 235 },
    { "ImageSmallerThanKernelWidth", 236 },
    { "ImageSmallerThanRadius", 237 },
    { "ImageWidthsOrHeightsDiffer", 238 },
    { "InputImagesAlreadySpecified", 239 },
    { "InvalidSubimageSpecification", 240 },
    { "KernelRadiusIsTooSmall", 241 },
    { "KernelWidthMustBeAnOddNumber", 242 },
    { "MatrixIsNotSquare", 243 },
    { "MatrixOrderOutOfRange", 244 },
    { "MissingAnImageFilename", 245 },
    { "MissingArgument", 246 },
    { "MustSpecifyAnImageName", 247 },
    { "MustSpecifyImageSize", 248 },
    { "NoBlobDefined", 249 },
    { "NoImagesDefined", 250 },
    { "NonzeroWidthAndHeightRequired", 251 },
    { "NoProfileNameWasGiven", 252 },
    { "NullBlobArgument", 253 },
    { "ReferenceImageRequired", 254 },
    { "ReferenceIsNotMyType", 255 },
    { "RequestDidNotReturnAnImage", 256 },
    { "SteganoImageRequired", 257 },
    { "StereoImageRequired", 258 },
    { "SubimageSpecificationReturnsNoImages", 259 },
    { "UnableToAddOrRemoveProfile", 260 },
    { "UnableToAverageImageSequence", 261 },
    { "UnableToBlurImage", 262 },
    { "UnableToChopImage", 263 },
    { "UnableToColorMatrixImage", 264 },
    { "UnableToConstituteImage", 265 },
    { "UnableToConvolveImage", 266 },
    { "UnableToEdgeImage", 267 },
    { "UnableToEqualizeImage", 268 },
    { "UnableToFilterImage", 269 },
    { "UnableToFormatImageMetadata", 270 },
    { "UnableToFrameImage", 271 },
    { "UnableToOilPaintImage", 272 },
    { "UnableToPaintImage", 273 },
    { "UnableToRaiseImage", 274 },
    { "UnableToSharpenImage", 275 },
    { "UnableToThresholdImage", 276 },
    { "UnableToWaveImage", 277 },
    { "UnrecognizedAttribute", 278 },
    { "UnrecognizedChannelType", 279 },
    { "UnrecognizedColor", 280 },
    { "UnrecognizedColormapType", 281 },
    { "UnrecognizedColorspace", 282 },
    { "UnrecognizedCommand", 283 },
    { "UnrecognizedComposeOperator", 284 },
    { "UnrecognizedDisposeMethod", 285 },
    { "UnrecognizedElement", 286 },
    { "UnrecognizedEndianType", 287 },
    { "UnrecognizedGravityType", 288 },
    { "UnrecognizedHighlightStyle", 289 },
    { "UnrecognizedImageCompression", 290 },
    { "UnrecognizedImageFilter", 291 },
    { "UnrecognizedImageFormat", 292 },
    { "UnrecognizedImageMode", 293 },
    { "UnrecognizedImageType", 294 },
    { "UnrecognizedIntentType", 295 },
    { "UnrecognizedInterlaceType", 296 },
    { "UnrecognizedListType", 297 },
    { "UnrecognizedMetric", 298 },
    { "UnrecognizedModeType", 299 },
    { "UnrecognizedNoiseType", 300 },
    { "UnrecognizedOperator", 301 },
    { "UnrecognizedOption", 302 },
    { "UnrecognizedPerlMagickMethod", 303 },
    { "UnrecognizedPixelMap", 304 },
    { "UnrecognizedPreviewType", 305 },
    { "UnrecognizedResourceType", 306 },
    { "UnrecognizedType", 307 },
    { "UnrecognizedUnitsType", 308 },
    { "UnrecognizedVirtualPixelMethod", 309 },
    { "UnsupportedSamplingFactor", 310 },
    { "UsageError", 311 },
    { "InvalidColorspaceType", 312 },
    { "InvalidEndianType", 313 },
    { "InvalidImageType", 314 },
    { "InvalidInterlaceType", 315 },
    { "MissingAnImageFilename", 316 },
    { "MissingArgument", 317 },
    { "NoImagesWereLoaded", 318 },
    { "OptionLengthExceedsLimit", 319 },
    { "RequestDidNotReturnAnImage", 320 },
    { "UnableToOpenXServer", 321 },
    { "UnableToPersistKey", 322 },
    { "UnrecognizedColormapType", 323 },
    { "UnrecognizedColorspaceType", 324 },
    { "UnrecognizedDisposeMethod", 325 },
    { "UnrecognizedEndianType", 326 },
    { "UnrecognizedFilterType", 327 },
    { "UnrecognizedImageCompressionType", 328 },
    { "UnrecognizedImageType", 329 },
    { "UnrecognizedInterlaceType", 330 },
    { "UnrecognizedOption", 331 },
    { "UnrecognizedResourceType", 332 },
    { "UnrecognizedVirtualPixelMethod", 333 },
    { "UnrecognizedColor", 334 },
    { "ImageExpected", 335 },
    { "ImageInfoExpected", 336 },
    { "StructureSizeMismatch", 337 },
    { "UnableToGetRegistryID", 338 },
    { "UnableToLocateImage", 339 },
    { "UnableToSetRegistry", 340 },
    { "Default", 341 },
    { "Default", 342 },
    { "CacheResourcesExhausted", 343 },
    { "ImagePixelLimitExceeded", 344 },
    { "MemoryAllocationFailed", 345 },
    { "NoPixelsDefinedInCache", 346 },
    { "PixelCacheAllocationFailed", 347 },
    { "UnableToAddColorProfile", 348 },
    { "UnableToAddGenericProfile", 349 },
    { "UnableToAddIPTCProfile", 350 },
    { "UnableToAllocateCoefficients", 351 },
    { "UnableToAllocateColormap", 352 },
    { "UnableToAllocateICCProfile", 353 },
    { "UnableToAllocateString", 354 },
    { "UnableToAnnotateImage", 355 },
    { "UnableToAverageImageSequence", 356 },
    { "UnableToCloneDrawingWand", 357 },
    { "UnableToCloneImage", 358 },
    { "UnableToComputeImageSignature", 359 },
    { "UnableToConstituteImage", 360 },
    { "UnableToConvertFont", 361 },
    { "UnableToConvertStringToTokens", 362 },
    { "UnableToCreateColormap", 363 },
    { "UnableToCreateColorTransform", 364 },
    { "UnableToCreateCommandWidget", 365 },
    { "UnableToCreateImageGroup", 366 },
    { "UnableToCreateImageMontage", 367 },
    { "UnableToCreateXWindow", 368 },
    { "UnableToCropImage", 369 },
    { "UnableToDespeckleImage", 370 },
    { "UnableToDetermineImageClass", 371 },
    { "UnableToDetermineTheNumberOfImageColors", 372 },
    { "UnableToDitherImage", 373 },
    { "UnableToDrawOnImage", 374 },
    { "UnableToEdgeImage", 375 },
    { "UnableToEmbossImage", 376 },
    { "UnableToEnhanceImage", 377 },
    { "UnableToFloodfillImage", 378 },
    { "UnableToGammaCorrectImage", 379 },
    { "UnableToGetBestIconSize", 380 },
    { "UnableToGetFromRegistry", 381 },
    { "UnableToGetPackageInfo", 382 },
    { "UnableToLevelImage", 383 },
    { "UnableToMagnifyImage", 384 },
    { "UnableToManageColor", 385 },
    { "UnableToMapImage", 386 },
    { "UnableToMapImageSequence", 387 },
    { "UnableToMedianFilterImage", 388 },
    { "UnableToMotionBlurImage", 389 },
    { "UnableToNoiseFilterImage", 390 },
    { "UnableToNormalizeImage", 391 },
    { "UnableToOpenColorProfile", 392 },
    { "UnableToQuantizeImage", 393 },
    { "UnableToQuantizeImageSequence", 394 },
    { "UnableToReadTextChunk", 395 },
    { "UnableToReadXImage", 396 },
    { "UnableToReadXServerColormap", 397 },
    { "UnableToResizeImage", 398 },
    { "UnableToRotateImage", 399 },
    { "UnableToSampleImage", 400 },
    { "UnableToScaleImage", 401 },
    { "UnableToSelectImage", 402 },
    { "UnableToSharpenImage", 403 },
    { "UnableToShaveImage", 404 },
    { "UnableToShearImage", 405 },
    { "UnableToSortImageColormap", 406 },
    { "UnableToThresholdImage", 407 },
    { "UnableToTransformColorspace", 408 },
    { "MemoryAllocationFailed", 409 },
    { "UnableToAllocateAscii85Info", 410 },
    { "UnableToAllocateCacheInfo", 411 },
    { "UnableToAllocateCacheView", 412 },
    { "UnableToAllocateColorInfo", 413 },
    { "UnableToAllocateDashPattern", 414 },
    { "UnableToAllocateDelegateInfo", 415 },
    { "UnableToAllocateDerivatives", 416 },
    { "UnableToAllocateDrawContext", 417 },
    { "UnableToAllocateDrawInfo", 418 },
    { "UnableToAllocateDrawingWand", 419 },
    { "UnableToAllocateGammaMap", 420 },
    { "UnableToAllocateImage", 421 },
    { "UnableToAllocateImagePixels", 422 },
    { "UnableToAllocateLogInfo", 423 },
    { "UnableToAllocateMagicInfo", 424 },
    { "UnableToAllocateMagickInfo", 425 },
    { "UnableToAllocateModuleInfo", 426 },
    { "UnableToAllocateMontageInfo", 427 },
    { "UnableToAllocateQuantizeInfo", 428 },
    { "UnableToAllocateRandomKernel", 429 },
    { "UnableToAllocateRegistryInfo", 430 },
    { "UnableToAllocateSemaphoreInfo", 431 },
    { "UnableToAllocateString", 432 },
    { "UnableToAllocateTypeInfo", 433 },
    { "UnableToAllocateWand", 434 },
    { "UnableToAnimateImageSequence", 435 },
    { "UnableToCloneBlobInfo", 436 },
    { "UnableToCloneCacheInfo", 437 },
    { "UnableToCloneImage", 438 },
    { "UnableToCloneImageInfo", 439 },
    { "UnableToConcatenateString", 440 },
    { "UnableToConvertText", 441 },
    { "UnableToCreateColormap", 442 },
    { "UnableToDisplayImage", 443 },
    { "UnableToEscapeString", 444 },
    { "UnableToInterpretMSLImage", 445 },
    { "MemoryAllocationFailed", 446 },
    { "ImageDoesNotContainTheStreamGeometry", 447 },
    { "NoStreamHandlerIsDefined", 448 },
    { "PixelCacheIsNotOpen", 449 },
    { "UnableToAcquirePixelStream", 450 },
    { "UnableToSetPixelStream", 451 },
    { "UnableToSyncPixelStream", 452 },
    { "Default", 453 },
    { "Default", 454 },
    { "FontSubstitutionRequired", 455 },
    { "UnableToGetTypeMetrics", 456 },
    { "UnableToInitializeFreetypeLibrary", 457 },
    { "UnableToReadFont", 458 },
    { "UnrecognizedFontEncoding", 459 },
    { "Default", 460 },
    { "Default", 461 },
    { "InvalidColormapIndex", 462 },
    { "WandAPINotImplemented", 463 },
    { "WandContainsNoImageIndexs", 464 },
    { "WandContainsNoImages", 465 },
    { "ColorIsNotKnownToServer", 466 },
    { "NoWindowWithSpecifiedIDExists", 467 },
    { "StandardColormapIsNotInitialized", 468 },
    { "UnableToConnectToRemoteDisplay", 469 },
    { "UnableToCreateBitmap", 470 },
    { "UnableToCreateColormap", 471 },
    { "UnableToCreatePixmap", 472 },
    { "UnableToCreateProperty", 473 },
    { "UnableToCreateStandardColormap", 474 },
    { "UnableToDisplayImageInfo", 475 },
    { "UnableToGetProperty", 476 },
    { "UnableToGetStandardColormap", 477 },
    { "UnableToGetVisual", 478 },
    { "UnableToGrabMouse", 479 },
    { "UnableToLoadFont", 480 },
    { "UnableToMatchVisualToStandardColormap", 481 },
    { "UnableToOpenXServer", 482 },
    { "UnableToReadXAttributes", 483 },
    { "UnableToReadXWindowImage", 484 },
    { "UnrecognizedColormapType", 485 },
    { "UnrecognizedGravityType", 486 },
    { "UnrecognizedVisualSpecifier", 487 },
    { "UnableToAllocateXHints", 488 },
    { "UnableToCreateCursor", 489 },
    { "UnableToCreateGraphicContext", 490 },
    { "UnableToCreateStandardColormap", 491 },
    { "UnableToCreateTextProperty", 492 },
    { "UnableToCreateXImage", 493 },
    { "UnableToCreateXPixmap", 494 },
    { "UnableToCreateXWindow", 495 },
    { "UnableToDisplayImage", 496 },
    { "UnableToDitherImage", 497 },
    { "UnableToGetPixelInfo", 498 },
    { "UnableToGetVisual", 499 },
    { "UnableToLoadFont", 500 },
    { "UnableToMakeXWindow", 501 },
    { "UnableToOpenXServer", 502 },
    { "UnableToViewFonts", 503 },
    { "UnableToGetVisual", 504 },
    { "UsingDefaultVisual", 505 },
    { 0, 0 }
  };
#endif

#if defined(_INCLUDE_MESSAGE_TABLE_)
static const char *message_dat[] =
  {
    "%1",
    "Unable to create blob",
    "Unable to open file",
    "Unable to read file",
    "Unable to read to offset",
    "Unable to seek to offset",
    "Unable to write blob",
    "Unrecognized image format",
    "default error",
    "default warning",
    "Inconsistent persistent cache depth",
    "Pixel cache is not open",
    "Unable to allocate cache view",
    "Unable to clone cache",
    "Unable to extend cache",
    "Unable to get cache nexus",
    "Unable to get pixels from cache",
    "Unable to open cache",
    "Unable to persist pixel cache",
    "Unable to read pixel cache",
    "Unable to sync cache (check temporary file disk space)",
    "disk allocation failed",
    "Unable to extend pixel cache",
    "default warning",
    "Colormap size exceeds limit",
    "Colormap type not supported",
    "Colorspace model is not supported",
    "Color type not supported",
    "Compression not valid",
    "Data encoding scheme is not supported",
    "Data storage type is not supported",
    "Delta-PNG is not supported",
    "Encrypted WPG image file not supported",
    "Fractal compression not supported",
    "Image column or row size is not supported",
    "Image does not have a matte channel",
    "Image is not tiles",
    "Image type not supported",
    "Incompatible size of double",
    "Irregular channel geometry not supported",
    "JNG compression is not supported",
    "JPEG compression is not supported",
    "JPEG embedding failed",
    "Location type is not supported",
    "Map storage type is not supported",
    "MSB order not supported bitmap",
    "Multi-dimensional matrices are not supported",
    "Multiple record list not supported",
    "No 8BIM data is available",
    "No APP1 data is available",
    "No bitmap on clipboard",
    "No color profile available",
    "No data returned",
    "No image vector graphics",
    "No IPTC info was found",
    "No IPTC profile available",
    "Number of images is not supported",
    "Only continuous tone picture supported",
    "Only level zero files Supported",
    "PNG compression is not supported",
    "PNG library is too old",
    "RLE compression not supported",
    "Subsampling requires that image width be evenly divisible by two",
    "Unable to copy profile",
    "Unable to create a DC",
    "Unable to create bitmap",
    "Unable to decompress image",
    "Unable to Initialize FPX library",
    "Unable to open blob",
    "Unable to read aspect ratio",
    "Unable to read CIELAB images",
    "Unable to read summary info",
    "Unable to set affine matrix",
    "Unable to set aspect ratio",
    "Unable to set color twist",
    "Unable to set contrast",
    "Unable to set filtering value",
    "Unable to set image comment",
    "Unable to set image title",
    "Unable to set JPEG level",
    "Unable to set region of interest",
    "Unable to set summary info",
    "Unable to translate text",
    "Unable to write MPEG parameters",
    "Unable to zip-compress image",
    "Unsupported cell type in the matrix",
    "ZIP compression is not supported",
    "default error",
    "Lossless to lossy JPEG conversion",
    "include element nested too deeply",
    "Registry key lookup failed. Package is not properly installed on this machine.",
    "String token maximum length exceeded",
    "Unable to access configuration file",
    "Unable to access font file",
    "Unable to access log configuration file",
    "Unable to access module file",
    "default error",
    "default warning",
    "An error has occurred reading from file",
    "An error has occurred writing to file",
    "Colormap exceeded 256 colors",
    "Corrupt image",
    "Image file or does not contain any image data",
    "Image type not supported",
    "Improper image header",
    "Insufficient image data in file",
    "Invalid colormap index",
    "invalid file format version",
    "Length and filesize do not match",
    "Missing a required image channel",
    "Negative or zero image size",
    "Non OS2 BMP header size less than 40",
    "Not enough tiles found in level",
    "Static planes value not equal to 1",
    "Subsampling requires that image width be evenly divisible by two",
    "Too much image data in file",
    "Unable to read colormap from dump file",
    "Unable to read color profile",
    "Unable to read extension block",
    "Unable to read generic profile",
    "Unable to read image data",
    "Unable to read image header",
    "Unable to read IPTC profile",
    "Unable to read pixmap from dump file",
    "Unable to read sub image data",
    "Unable to read VID image",
    "Unable to read window name from dump file",
    "Unable to runlength decode image",
    "Unable to uncompress image",
    "Unexpected end-of-file",
    "Unexpected sampling factor",
    "Unknown pattern type",
    "Unrecognized bits per pixel",
    "Unrecognized compression",
    "Unrecognized XWD header",
    "Unable to persist key",
    "Compression not valid",
    "Improper image header",
    "Negative or zero image size",
    "Non OS2 header size error",
    "Corrupt PCD image, skipping to sync byte",
    "Static planes value not equal to one",
    "Unrecognized bits per pixel",
    "Unrecognized image compression",
    "Unrecognized number of colors",
    "Delegate failed",
    "Failed to allocate argument list.",
    "Failed to allocate Ghostscript interpreter.",
    "Failed to compute output size",
    "Failed to render file",
    "Failed to scan file",
    "No tag found",
    "Postscript delegate failed",
    "Unable to create image",
    "Unable to create image component",
    "Unable to decode image file",
    "Unable to encode image file",
    "Unable to initialize FPX library",
    "Unable to initialize WMF library",
    "Unable to manage JP2 stream",
    "Unable to write SVG format",
    "default error",
    "default warning",
    "Already pushing pattern definition",
    "Non-conforming drawing primitive definition",
    "Unable to print",
    "unbalanced graphic context push-pop",
    "default error",
    "Not a relative URL",
    "Not currently pushing pattern definition",
    "URL not found",
    "Unable to create temporary file",
    "Unable to open file",
    "Unable to write file",
    "default error",
    "default warning",
    "angle is discontinuous",
    "Colorspace color profile mismatch",
    "image colorspace differs",
    "image difference exceeds limit (%s)",
    "image does not contain resolution",
    "image opacity differs",
    "Image sequence is required",
    "image size differs",
    "Invalid colormap index",
    "left and right image sizes differ",
    "no images were found",
    "no images were loaded",
    "no [LOCALE] image attribute",
    "too many cluster",
    "unable to append image",
    "Unable to assign profile",
    "unable to average image",
    "unable to coalesce image",
    "unable to compare images",
    "unable to create image mosaic",
    "unable to create stereo image",
    "unable to deconstruct image sequence",
    "unable to flatten image",
    "Unable to get clip mask",
    "unable to resize image",
    "unable to segment image",
    "Unable to set clip mask",
    "unable to shear image",
    "width or height exceeds limit",
    "Unable to persist key",
    "default warning",
    "DPS library is not available",
    "FPX library is not available",
    "FreeType library is not available",
    "JPEG compression library is not available",
    "LCMS encoding not enabled",
    "LZW encoding not enabled",
    "No decode delegate for this image format",
    "No encode delegate for this image format",
    "TIFF library is not available",
    "XML library is not available",
    "X Window library is not available",
    "ZLIB compression library is not available",
    "default error",
    "default warning",
    "Failed to close module",
    "Failed to find symbol",
    "Unable to load module",
    "Unable to register image format",
    "Unrecognized module",
    "Unable to initialize module loader",
    "default warning",
    "bevel width is negative",
    "color separated image required",
    "frame is less than image size",
    "geometry dimensions are zero",
    "geometry does not contain image",
    "hald clut image dimensions are invalid",
    "images are not the same size",
    "size must exceed bevel width",
    "image smaller than kernel width",
    "image smaller than radius",
    "image widths or heights differ",
    "input images already specified",
    "Invalid subimage specification",
    "kernel radius is too small",
    "kernel width must be an odd number",
    "Matrix is not square (%s elements)",
    "Matrix size is out of range",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
    "Must specify a image name",
    "Must specify image size",
    "No Binary Large OBjects defined",
    "No images defined",
    "Non-zero width and height required",
    "No profile name was given",
    "Null blob argument",
    "Reference image required",
    "Reference is not my type",
    "Request did not return an image",
    "Stegano image required",
    "Stereo image required",
    "Subimage specification returns no images",
    "Unable to add or remove profile",
    "unable to average image sequence",
    "unable to blur image",
    "unable to chop image",
    "Unable to color matrix image",
    "Unable to constitute image",
    "Unable to convolve image",
    "Unable to edge image",
    "Unable to equalize image",
    "Unable to filter image",
    "unable to format image meta data",
    "Unable to frame image",
    "unable to oil paint image",
    "Unable to paint image",
    "Unable to raise image",
    "Unable to sharpen image",
    "Unable to threshold image",
    "Unable to wave image",
    "Unrecognized attribute",
    "Unrecognized channel type",
    "Unrecognized color",
    "Unrecognized colormap type",
    "Unrecognized image colorspace",
    "Unrecognized command '%s'. Use -help for a usage summary or see manual.",
    "Unrecognized compose operator",
    "Unrecognized dispose method",
    "Unrecognized element",
    "Unrecognized endian type",
    "Unrecognized gravity type",
    "Unrecognized highlight style",
    "Unrecognized image compression",
    "Unrecognized image filter",
    "Unrecognized image format",
    "Unrecognized image mode",
    "Unrecognized image type",
    "Unrecognized intent type",
    "Unrecognized interlace type",
    "Unrecognized list type",
    "Unrecognized error metric",
    "Unrecognized mode type",
    "Unrecognized noise type",
    "Unrecognized operator",
    "Unrecognized option",
    "Unrecognized PerlMagick method",
    "Unrecognized pixel map",
    "Unrecognized preview type",
    "Unrecognized resource type",
    "Unrecognized type",
    "Unrecognized units type",
    "Unrecognized virtual pixel method",
    "Unsupported sampling factor",
    "Improper arguments supplied, please see manual",
    "Invalid colorspace type",
    "Invalid endian type",
    "Invalid image type",
    "Invalid interlace type",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
    "No images were loaded",
    "Option length exceeds limit",
    "Request did not return an image",
    "Unable to open XServer",
    "Unable to persist key",
    "Unrecognized colormap type",
    "Unrecognized colorspace type",
    "unrecognized dispose method",
    "Unrecognized endian type",
    "Unrecognized filter type",
    "unrecognized compression type",
    "Unrecognized image type",
    "Unrecognized interlace type",
    "Unrecognized option",
    "Unrecognized resource type",
    "Unrecognized virtual pixel method",
    "Unrecognized color",
    "image expected",
    "image info expected",
    "structure size mismatch",
    "Unable to get registry ID",
    "Unable to locate image",
    "Unable to set registry",
    "default error",
    "default warning",
    "Disk space limit exceeded (see -limit Disk)",
    "Image pixel limit exceeded (see -limit Pixels)",
    "Memory allocation failed",
    "No pixels defined in cache",
    "Pixel cache allocation failed",
    "unable to add ICC Color profile",
    "unable to add generic profile",
    "unable to add IPTC profile",
    "unable to allocate coefficients",
    "Unable to allocate colormap",
    "unable to allocate ICC profile",
    "unable to allocate string",
    "Unable to annotate image",
    "unable to average image sequence",
    "unable to clone drawing wand",
    "unable to clone image",
    "unable to compute image signature",
    "unable to constitute image",
    "unable to convert font",
    "unable to convert strings to tokens",
    "Unable to create colormap",
    "unable to create color transform",
    "unable to create command widget",
    "unable to create image group",
    "Unable to create image montage",
    "unable to create X window",
    "unable to crop image",
    "unable to despeckle image",
    "unable to determine image class",
    "unable to determine the number of image colors",
    "unable to dither image",
    "unable to draw on image",
    "unable to edge image",
    "unable to emboss image",
    "unable to enhance image",
    "unable to floodfill image",
    "unable to gamma correct image",
    "unable to get best icon size",
    "unable to get from registry",
    "Unable to get package info",
    "unable to level image",
    "unable to magnify image",
    "Unable to manage color",
    "Unable to map image",
    "Unable to map image sequence",
    "unable to median filter image",
    "unable to motion blur image",
    "unable to noise filter image",
    "unable to normalize image",
    "unable to open color profile",
    "unable to quantize image",
    "unable to quantize image sequence",
    "unable to read text chunk",
    "unable to read X image",
    "unable to read X server colormap",
    "unable to resize image",
    "unable to rotate image",
    "unable to sample image",
    "unable to scale image",
    "unable to select image",
    "unable to sharpen image",
    "unable to shave image",
    "unable to shear image",
    "unable to sort image colormap",
    "unable to threshold image",
    "unable to transform colorspace",
    "Memory allocation failed",
    "unable to allocate ascii85 info",
    "unable to allocate cache info",
    "unable to allocate cache view",
    "unable to allocate color info",
    "unable to allocate dash pattern",
    "unable to allocate delegate info",
    "unable to allocate derivates",
    "unable to allocate draw context",
    "unable to allocate draw info",
    "unable to allocate drawing wand",
    "unable to allocate gamma map",
    "unable to allocate image",
    "unable to allocate image pixels",
    "unable to allocate log info",
    "unable to allocate magic info",
    "unable to allocate magick info",
    "unable to allocate module info",
    "unable to allocate montage info",
    "unable to allocate quantize info",
    "unable to allocate random kernel",
    "unable to allocate registry info",
    "unable to allocate semaphore info",
    "unable to allocate string",
    "unable to allocate type info",
    "unable to allocate wand",
    "unable to animate image sequence",
    "unable to clone blob info",
    "unable to clone cache info",
    "unable to clone image",
    "unable to clone image info",
    "unable to concatenate string",
    "unable to convert text",
    "unable to create colormap",
    "unable to display image",
    "unable to escape string",
    "unable to interpret MSL image",
    "Memory allocation failed",
    "image does not contain the stream geometry",
    "no stream handler is defined",
    "Pixel cache is not open",
    "Unable to acquire pixel stream",
    "Unable to set pixel stream",
    "Unable to sync pixel stream",
    "default error",
    "default warning",
    "Font substitution required",
    "Unable to get type metrics",
    "Unable to initialize freetype library",
    "Unable to read font",
    "Unrecognized font encoding",
    "default error",
    "default warning",
    "invalid colormap index `%.1024s",
    "Wand API not implemented `%.1024s",
    "Wand contains no image indices `%.1024s",
    "Wand contains no images `%.1024s",
    "Color is not known to server",
    "No window with specified ID exists",
    "Standard Colormap is not initialized",
    "Unable to connect to remote display",
    "Unable to create bitmap",
    "Unable to create colormap",
    "Unable to create pixmap",
    "Unable to create property",
    "Unable to create standard colormap",
    "Unable to display image info",
    "Unable to get property",
    "Unable to get Standard Colormap",
    "Unable to get visual",
    "Unable to grab mouse",
    "Unable to load font",
    "Unable to match visual to Standard Colormap",
    "Unable to open X server",
    "Unable to read X attributes",
    "Unable to read X window image",
    "Unrecognized colormap type",
    "Unrecognized gravity type",
    "Unrecognized visual specifier",
    "Unable to allocate X hints",
    "Unable to create X cursor",
    "Unable to create graphic context",
    "unable to create standard colormap",
    "Unable to create text property",
    "Unable to create X image",
    "Unable to create X pixmap",
    "Unable to create X window",
    "unable to display image",
    "unable to dither image",
    "Unable to get pixel info",
    "Unable to get visual",
    "Unable to load font",
    "Unable to make X window",
    "Unable to open X server",
    "Unable to view fonts",
    "Unable to get visual",
    "UsingDefaultVisual",
    0
  };
#endif
