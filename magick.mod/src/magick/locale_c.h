#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 522

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
#define MGK_ConfigureFatalErrorUnableToChangeToWorkingDirectory 97
#define MGK_ConfigureFatalErrorUnableToGetCurrentDirectory 98
#define MGK_ConfigureFatalErrorUnableToRestoreCurrentDirectory 99
#define MGK_ConfigureWarningDefault 100
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 101
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 102
#define MGK_CorruptImageErrorColormapExceeds256Colors 103
#define MGK_CorruptImageErrorCorruptImage 104
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 105
#define MGK_CorruptImageErrorImageTypeNotSupported 106
#define MGK_CorruptImageErrorImproperImageHeader 107
#define MGK_CorruptImageErrorInsufficientImageDataInFile 108
#define MGK_CorruptImageErrorInvalidColormapIndex 109
#define MGK_CorruptImageErrorInvalidFileFormatVersion 110
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 111
#define MGK_CorruptImageErrorMissingImageChannel 112
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 113
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 114
#define MGK_CorruptImageErrorNotEnoughTiles 115
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 116
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 117
#define MGK_CorruptImageErrorTooMuchImageDataInFile 118
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 119
#define MGK_CorruptImageErrorUnableToReadColorProfile 120
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 121
#define MGK_CorruptImageErrorUnableToReadGenericProfile 122
#define MGK_CorruptImageErrorUnableToReadImageData 123
#define MGK_CorruptImageErrorUnableToReadImageHeader 124
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 125
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 126
#define MGK_CorruptImageErrorUnableToReadSubImageData 127
#define MGK_CorruptImageErrorUnableToReadVIDImage 128
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 129
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 130
#define MGK_CorruptImageErrorUnableToUncompressImage 131
#define MGK_CorruptImageErrorUnexpectedEndOfFile 132
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 133
#define MGK_CorruptImageErrorUnknownPatternType 134
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 135
#define MGK_CorruptImageErrorUnrecognizedImageCompression 136
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 137
#define MGK_CorruptImageFatalErrorUnableToPersistKey 138
#define MGK_CorruptImageWarningCompressionNotValid 139
#define MGK_CorruptImageWarningImproperImageHeader 140
#define MGK_CorruptImageWarningLengthAndFilesizeDoNotMatch 141
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 142
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 143
#define MGK_CorruptImageWarningSkipToSyncByte 144
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 145
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 146
#define MGK_CorruptImageWarningUnrecognizedImageCompression 147
#define MGK_CorruptImageWarningUnrecognizedNumberOfColors 148
#define MGK_DelegateErrorDelegateFailed 149
#define MGK_DelegateErrorFailedToAllocateArgumentList 150
#define MGK_DelegateErrorFailedToAllocateGhostscriptInterpreter 151
#define MGK_DelegateErrorFailedToComputeOutputSize 152
#define MGK_DelegateErrorFailedToRenderFile 153
#define MGK_DelegateErrorFailedToScanFile 154
#define MGK_DelegateErrorNoTagFound 155
#define MGK_DelegateErrorPostscriptDelegateFailed 156
#define MGK_DelegateErrorUnableToCreateImage 157
#define MGK_DelegateErrorUnableToCreateImageComponent 158
#define MGK_DelegateErrorUnableToDecodeImageFile 159
#define MGK_DelegateErrorUnableToEncodeImageFile 160
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 161
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 162
#define MGK_DelegateErrorUnableToManageJP2Stream 163
#define MGK_DelegateErrorUnableToWriteSVGFormat 164
#define MGK_DelegateFatalErrorDefault 165
#define MGK_DelegateWarningDefault 166
#define MGK_DrawErrorAlreadyPushingPatternDefinition 167
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 168
#define MGK_DrawErrorTooManyCoordinates 169
#define MGK_DrawErrorUnableToPrint 170
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 171
#define MGK_DrawFatalErrorDefault 172
#define MGK_DrawWarningNotARelativeURL 173
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 174
#define MGK_DrawWarningURLNotFound 175
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 176
#define MGK_FileOpenErrorUnableToOpenFile 177
#define MGK_FileOpenErrorUnableToWriteFile 178
#define MGK_FileOpenFatalErrorDefault 179
#define MGK_FileOpenWarningDefault 180
#define MGK_ImageErrorAngleIsDiscontinuous 181
#define MGK_ImageErrorColorspaceColorProfileMismatch 182
#define MGK_ImageErrorImageColorspaceDiffers 183
#define MGK_ImageErrorImageColorspaceMismatch 184
#define MGK_ImageErrorImageDifferenceExceedsLimit 185
#define MGK_ImageErrorImageDoesNotContainResolution 186
#define MGK_ImageErrorImageOpacityDiffers 187
#define MGK_ImageErrorImageSequenceIsRequired 188
#define MGK_ImageErrorImageSizeDiffers 189
#define MGK_ImageErrorInvalidColormapIndex 190
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 191
#define MGK_ImageErrorNoImagesWereFound 192
#define MGK_ImageErrorNoImagesWereLoaded 193
#define MGK_ImageErrorNoLocaleImageAttribute 194
#define MGK_ImageErrorTooManyClusters 195
#define MGK_ImageErrorUnableToAppendImage 196
#define MGK_ImageErrorUnableToAssignProfile 197
#define MGK_ImageErrorUnableToAverageImage 198
#define MGK_ImageErrorUnableToCoalesceImage 199
#define MGK_ImageErrorUnableToCompareImages 200
#define MGK_ImageErrorUnableToCreateImageMosaic 201
#define MGK_ImageErrorUnableToCreateStereoImage 202
#define MGK_ImageErrorUnableToDeconstructImageSequence 203
#define MGK_ImageErrorUnableToFlattenImage 204
#define MGK_ImageErrorUnableToGetClipMask 205
#define MGK_ImageErrorUnableToHandleImageChannel 206
#define MGK_ImageErrorUnableToResizeImage 207
#define MGK_ImageErrorUnableToSegmentImage 208
#define MGK_ImageErrorUnableToSetClipMask 209
#define MGK_ImageErrorUnableToShearImage 210
#define MGK_ImageErrorWidthOrHeightExceedsLimit 211
#define MGK_ImageFatalErrorUnableToPersistKey 212
#define MGK_ImageWarningDefault 213
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 214
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 215
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 216
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 217
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 218
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 219
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 220
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 221
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 222
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 223
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 224
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 225
#define MGK_MissingDelegateFatalErrorDefault 226
#define MGK_MissingDelegateWarningDefault 227
#define MGK_ModuleErrorFailedToCloseModule 228
#define MGK_ModuleErrorFailedToFindSymbol 229
#define MGK_ModuleErrorUnableToLoadModule 230
#define MGK_ModuleErrorUnableToRegisterImageFormat 231
#define MGK_ModuleErrorUnrecognizedModule 232
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 233
#define MGK_ModuleWarningDefault 234
#define MGK_MonitorErrorDefault 235
#define MGK_MonitorFatalErrorDefault 236
#define MGK_MonitorFatalErrorUserRequestedTerminationBySignal 237
#define MGK_MonitorWarningDefault 238
#define MGK_OptionErrorBevelWidthIsNegative 239
#define MGK_OptionErrorColorSeparatedImageRequired 240
#define MGK_OptionErrorFrameIsLessThanImageSize 241
#define MGK_OptionErrorGeometryDimensionsAreZero 242
#define MGK_OptionErrorGeometryDoesNotContainImage 243
#define MGK_OptionErrorHaldClutImageDimensionsInvalid 244
#define MGK_OptionErrorImagesAreNotTheSameSize 245
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 246
#define MGK_OptionErrorImageSmallerThanKernelWidth 247
#define MGK_OptionErrorImageSmallerThanRadius 248
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 249
#define MGK_OptionErrorInputImagesAlreadySpecified 250
#define MGK_OptionErrorInvalidSubimageSpecification 251
#define MGK_OptionErrorKernelRadiusIsTooSmall 252
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 253
#define MGK_OptionErrorMatrixIsNotSquare 254
#define MGK_OptionErrorMatrixOrderOutOfRange 255
#define MGK_OptionErrorMissingAnImageFilename 256
#define MGK_OptionErrorMissingArgument 257
#define MGK_OptionErrorMustSpecifyAnImageName 258
#define MGK_OptionErrorMustSpecifyImageSize 259
#define MGK_OptionErrorNoBlobDefined 260
#define MGK_OptionErrorNoImagesDefined 261
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 262
#define MGK_OptionErrorNoProfileNameWasGiven 263
#define MGK_OptionErrorNullBlobArgument 264
#define MGK_OptionErrorReferenceImageRequired 265
#define MGK_OptionErrorReferenceIsNotMyType 266
#define MGK_OptionErrorRegionAreaExceedsLimit 267
#define MGK_OptionErrorRequestDidNotReturnAnImage 268
#define MGK_OptionErrorSteganoImageRequired 269
#define MGK_OptionErrorStereoImageRequired 270
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 271
#define MGK_OptionErrorUnableToAddOrRemoveProfile 272
#define MGK_OptionErrorUnableToAverageImageSequence 273
#define MGK_OptionErrorUnableToBlurImage 274
#define MGK_OptionErrorUnableToChopImage 275
#define MGK_OptionErrorUnableToColorMatrixImage 276
#define MGK_OptionErrorUnableToConstituteImage 277
#define MGK_OptionErrorUnableToConvolveImage 278
#define MGK_OptionErrorUnableToEdgeImage 279
#define MGK_OptionErrorUnableToEqualizeImage 280
#define MGK_OptionErrorUnableToFilterImage 281
#define MGK_OptionErrorUnableToFormatImageMetadata 282
#define MGK_OptionErrorUnableToFrameImage 283
#define MGK_OptionErrorUnableToOilPaintImage 284
#define MGK_OptionErrorUnableToPaintImage 285
#define MGK_OptionErrorUnableToRaiseImage 286
#define MGK_OptionErrorUnableToSharpenImage 287
#define MGK_OptionErrorUnableToThresholdImage 288
#define MGK_OptionErrorUnableToWaveImage 289
#define MGK_OptionErrorUnrecognizedAttribute 290
#define MGK_OptionErrorUnrecognizedChannelType 291
#define MGK_OptionErrorUnrecognizedColor 292
#define MGK_OptionErrorUnrecognizedColormapType 293
#define MGK_OptionErrorUnrecognizedColorspace 294
#define MGK_OptionErrorUnrecognizedCommand 295
#define MGK_OptionErrorUnrecognizedComposeOperator 296
#define MGK_OptionErrorUnrecognizedDisposeMethod 297
#define MGK_OptionErrorUnrecognizedElement 298
#define MGK_OptionErrorUnrecognizedEndianType 299
#define MGK_OptionErrorUnrecognizedGravityType 300
#define MGK_OptionErrorUnrecognizedHighlightStyle 301
#define MGK_OptionErrorUnrecognizedImageCompression 302
#define MGK_OptionErrorUnrecognizedImageFilter 303
#define MGK_OptionErrorUnrecognizedImageFormat 304
#define MGK_OptionErrorUnrecognizedImageMode 305
#define MGK_OptionErrorUnrecognizedImageType 306
#define MGK_OptionErrorUnrecognizedIntentType 307
#define MGK_OptionErrorUnrecognizedInterlaceType 308
#define MGK_OptionErrorUnrecognizedListType 309
#define MGK_OptionErrorUnrecognizedMetric 310
#define MGK_OptionErrorUnrecognizedModeType 311
#define MGK_OptionErrorUnrecognizedNoiseType 312
#define MGK_OptionErrorUnrecognizedOperator 313
#define MGK_OptionErrorUnrecognizedOption 314
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 315
#define MGK_OptionErrorUnrecognizedPixelMap 316
#define MGK_OptionErrorUnrecognizedPreviewType 317
#define MGK_OptionErrorUnrecognizedResourceType 318
#define MGK_OptionErrorUnrecognizedType 319
#define MGK_OptionErrorUnrecognizedUnitsType 320
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 321
#define MGK_OptionErrorUnsupportedSamplingFactor 322
#define MGK_OptionErrorUsageError 323
#define MGK_OptionFatalErrorInvalidColorspaceType 324
#define MGK_OptionFatalErrorInvalidEndianType 325
#define MGK_OptionFatalErrorInvalidImageType 326
#define MGK_OptionFatalErrorInvalidInterlaceType 327
#define MGK_OptionFatalErrorMissingAnImageFilename 328
#define MGK_OptionFatalErrorMissingArgument 329
#define MGK_OptionFatalErrorNoImagesWereLoaded 330
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 331
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 332
#define MGK_OptionFatalErrorUnableToOpenXServer 333
#define MGK_OptionFatalErrorUnableToPersistKey 334
#define MGK_OptionFatalErrorUnrecognizedColormapType 335
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 336
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 337
#define MGK_OptionFatalErrorUnrecognizedEndianType 338
#define MGK_OptionFatalErrorUnrecognizedFilterType 339
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 340
#define MGK_OptionFatalErrorUnrecognizedImageType 341
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 342
#define MGK_OptionFatalErrorUnrecognizedOption 343
#define MGK_OptionFatalErrorUnrecognizedResourceType 344
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 345
#define MGK_OptionWarningUnrecognizedColor 346
#define MGK_RegistryErrorImageExpected 347
#define MGK_RegistryErrorImageInfoExpected 348
#define MGK_RegistryErrorStructureSizeMismatch 349
#define MGK_RegistryErrorUnableToGetRegistryID 350
#define MGK_RegistryErrorUnableToLocateImage 351
#define MGK_RegistryErrorUnableToSetRegistry 352
#define MGK_RegistryFatalErrorDefault 353
#define MGK_RegistryWarningDefault 354
#define MGK_ResourceLimitErrorCacheResourcesExhausted 355
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 356
#define MGK_ResourceLimitErrorMemoryAllocationFailed 357
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 358
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 359
#define MGK_ResourceLimitErrorUnableToAddColorProfile 360
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 361
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 362
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 363
#define MGK_ResourceLimitErrorUnableToAllocateColormap 364
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 365
#define MGK_ResourceLimitErrorUnableToAllocateString 366
#define MGK_ResourceLimitErrorUnableToAnnotateImage 367
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 368
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 369
#define MGK_ResourceLimitErrorUnableToCloneImage 370
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 371
#define MGK_ResourceLimitErrorUnableToConstituteImage 372
#define MGK_ResourceLimitErrorUnableToConvertFont 373
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 374
#define MGK_ResourceLimitErrorUnableToCreateColormap 375
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 376
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 377
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 378
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 379
#define MGK_ResourceLimitErrorUnableToCreateXWindow 380
#define MGK_ResourceLimitErrorUnableToCropImage 381
#define MGK_ResourceLimitErrorUnableToDespeckleImage 382
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 383
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 384
#define MGK_ResourceLimitErrorUnableToDitherImage 385
#define MGK_ResourceLimitErrorUnableToDrawOnImage 386
#define MGK_ResourceLimitErrorUnableToEdgeImage 387
#define MGK_ResourceLimitErrorUnableToEmbossImage 388
#define MGK_ResourceLimitErrorUnableToEnhanceImage 389
#define MGK_ResourceLimitErrorUnableToFloodfillImage 390
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 391
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 392
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 393
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 394
#define MGK_ResourceLimitErrorUnableToLevelImage 395
#define MGK_ResourceLimitErrorUnableToMagnifyImage 396
#define MGK_ResourceLimitErrorUnableToManageColor 397
#define MGK_ResourceLimitErrorUnableToMapImage 398
#define MGK_ResourceLimitErrorUnableToMapImageSequence 399
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 400
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 401
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 402
#define MGK_ResourceLimitErrorUnableToNormalizeImage 403
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 404
#define MGK_ResourceLimitErrorUnableToQuantizeImage 405
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 406
#define MGK_ResourceLimitErrorUnableToReadTextChunk 407
#define MGK_ResourceLimitErrorUnableToReadXImage 408
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 409
#define MGK_ResourceLimitErrorUnableToResizeImage 410
#define MGK_ResourceLimitErrorUnableToRotateImage 411
#define MGK_ResourceLimitErrorUnableToSampleImage 412
#define MGK_ResourceLimitErrorUnableToScaleImage 413
#define MGK_ResourceLimitErrorUnableToSelectImage 414
#define MGK_ResourceLimitErrorUnableToSharpenImage 415
#define MGK_ResourceLimitErrorUnableToShaveImage 416
#define MGK_ResourceLimitErrorUnableToShearImage 417
#define MGK_ResourceLimitErrorUnableToSortImageColormap 418
#define MGK_ResourceLimitErrorUnableToThresholdImage 419
#define MGK_ResourceLimitErrorUnableToTransformColorspace 420
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 421
#define MGK_ResourceLimitFatalErrorSemaporeOperationFailed 422
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 423
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 424
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 425
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 426
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 427
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 428
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 429
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 430
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 431
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 432
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 433
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 434
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 435
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 436
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 437
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 438
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 439
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 440
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 441
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 442
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 443
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 444
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 445
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 446
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 447
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 448
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 449
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 450
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 451
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 452
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 453
#define MGK_ResourceLimitFatalErrorUnableToConvertText 454
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 455
#define MGK_ResourceLimitFatalErrorUnableToDestroySemaphore 456
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 457
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 458
#define MGK_ResourceLimitFatalErrorUnableToInitializeSemaphore 459
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 460
#define MGK_ResourceLimitFatalErrorUnableToLockSemaphore 461
#define MGK_ResourceLimitFatalErrorUnableToUnlockSemaphore 462
#define MGK_ResourceLimitWarningMemoryAllocationFailed 463
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 464
#define MGK_StreamErrorNoStreamHandlerIsDefined 465
#define MGK_StreamErrorPixelCacheIsNotOpen 466
#define MGK_StreamErrorUnableToAcquirePixelStream 467
#define MGK_StreamErrorUnableToSetPixelStream 468
#define MGK_StreamErrorUnableToSyncPixelStream 469
#define MGK_StreamFatalErrorDefault 470
#define MGK_StreamWarningDefault 471
#define MGK_TypeErrorFontSubstitutionRequired 472
#define MGK_TypeErrorUnableToGetTypeMetrics 473
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 474
#define MGK_TypeErrorUnableToReadFont 475
#define MGK_TypeErrorUnrecognizedFontEncoding 476
#define MGK_TypeFatalErrorDefault 477
#define MGK_TypeWarningDefault 478
#define MGK_WandErrorInvalidColormapIndex 479
#define MGK_WandErrorWandAPINotImplemented 480
#define MGK_WandErrorWandContainsNoImageIndexs 481
#define MGK_WandErrorWandContainsNoImages 482
#define MGK_XServerErrorColorIsNotKnownToServer 483
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 484
#define MGK_XServerErrorStandardColormapIsNotInitialized 485
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 486
#define MGK_XServerErrorUnableToCreateBitmap 487
#define MGK_XServerErrorUnableToCreateColormap 488
#define MGK_XServerErrorUnableToCreatePixmap 489
#define MGK_XServerErrorUnableToCreateProperty 490
#define MGK_XServerErrorUnableToCreateStandardColormap 491
#define MGK_XServerErrorUnableToDisplayImageInfo 492
#define MGK_XServerErrorUnableToGetProperty 493
#define MGK_XServerErrorUnableToGetStandardColormap 494
#define MGK_XServerErrorUnableToGetVisual 495
#define MGK_XServerErrorUnableToGrabMouse 496
#define MGK_XServerErrorUnableToLoadFont 497
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 498
#define MGK_XServerErrorUnableToOpenXServer 499
#define MGK_XServerErrorUnableToReadXAttributes 500
#define MGK_XServerErrorUnableToReadXWindowImage 501
#define MGK_XServerErrorUnrecognizedColormapType 502
#define MGK_XServerErrorUnrecognizedGravityType 503
#define MGK_XServerErrorUnrecognizedVisualSpecifier 504
#define MGK_XServerFatalErrorUnableToAllocateXHints 505
#define MGK_XServerFatalErrorUnableToCreateCursor 506
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 507
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 508
#define MGK_XServerFatalErrorUnableToCreateTextProperty 509
#define MGK_XServerFatalErrorUnableToCreateXImage 510
#define MGK_XServerFatalErrorUnableToCreateXPixmap 511
#define MGK_XServerFatalErrorUnableToCreateXWindow 512
#define MGK_XServerFatalErrorUnableToDisplayImage 513
#define MGK_XServerFatalErrorUnableToDitherImage 514
#define MGK_XServerFatalErrorUnableToGetPixelInfo 515
#define MGK_XServerFatalErrorUnableToGetVisual 516
#define MGK_XServerFatalErrorUnableToLoadFont 517
#define MGK_XServerFatalErrorUnableToMakeXWindow 518
#define MGK_XServerFatalErrorUnableToOpenXServer 519
#define MGK_XServerFatalErrorUnableToViewFonts 520
#define MGK_XServerWarningUnableToGetVisual 521
#define MGK_XServerWarningUsingDefaultVisual 522

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
    { "Monitor", 33 },
    { "Option", 36 },
    { "Registry", 39 },
    { "Resource/Limit", 42 },
    { "Stream", 45 },
    { "Type", 48 },
    { "Wand", 51 },
    { "XServer", 52 },
    { 0, 54 }
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
    { "Configure/Warning", 99, ConfigureWarning },
    { "Corrupt/Image/Error", 100, CorruptImageError },
    { "Corrupt/Image/FatalError", 137, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 138, CorruptImageWarning },
    { "Delegate/Error", 148, DelegateError },
    { "Delegate/FatalError", 164, DelegateFatalError },
    { "Delegate/Warning", 165, DelegateWarning },
    { "Draw/Error", 166, DrawError },
    { "Draw/FatalError", 171, DrawFatalError },
    { "Draw/Warning", 172, DrawWarning },
    { "File/Open/Error", 175, FileOpenError },
    { "File/Open/FatalError", 178, FileOpenFatalError },
    { "File/Open/Warning", 179, FileOpenWarning },
    { "Image/Error", 180, ImageError },
    { "Image/FatalError", 211, ImageFatalError },
    { "Image/Warning", 212, ImageWarning },
    { "Missing/Delegate/Error", 213, MissingDelegateError },
    { "Missing/Delegate/FatalError", 225, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 226, MissingDelegateWarning },
    { "Module/Error", 227, ModuleError },
    { "Module/FatalError", 232, ModuleFatalError },
    { "Module/Warning", 233, ModuleWarning },
    { "Monitor/Error", 234, MonitorError },
    { "Monitor/FatalError", 235, MonitorFatalError },
    { "Monitor/Warning", 237, MonitorWarning },
    { "Option/Error", 238, OptionError },
    { "Option/FatalError", 323, OptionFatalError },
    { "Option/Warning", 345, OptionWarning },
    { "Registry/Error", 346, RegistryError },
    { "Registry/FatalError", 352, RegistryFatalError },
    { "Registry/Warning", 353, RegistryWarning },
    { "Resource/Limit/Error", 354, ResourceLimitError },
    { "Resource/Limit/FatalError", 420, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 462, ResourceLimitWarning },
    { "Stream/Error", 463, StreamError },
    { "Stream/FatalError", 469, StreamFatalError },
    { "Stream/Warning", 470, StreamWarning },
    { "Type/Error", 471, TypeError },
    { "Type/FatalError", 476, TypeFatalError },
    { "Type/Warning", 477, TypeWarning },
    { "Wand/Error", 478, WandError },
    { "XServer/Error", 482, XServerError },
    { "XServer/FatalError", 504, XServerFatalError },
    { "XServer/Warning", 520, XServerWarning },
    { 0, 522, UndefinedException }
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
    { "UnableToChangeToWorkingDirectory", 97 },
    { "UnableToGetCurrentDirectory", 98 },
    { "UnableToRestoreCurrentDirectory", 99 },
    { "Default", 100 },
    { "AnErrorHasOccurredReadingFromFile", 101 },
    { "AnErrorHasOccurredWritingToFile", 102 },
    { "ColormapExceeds256Colors", 103 },
    { "CorruptImage", 104 },
    { "ImageFileDoesNotContainAnyImageData", 105 },
    { "ImageTypeNotSupported", 106 },
    { "ImproperImageHeader", 107 },
    { "InsufficientImageDataInFile", 108 },
    { "InvalidColormapIndex", 109 },
    { "InvalidFileFormatVersion", 110 },
    { "LengthAndFilesizeDoNotMatch", 111 },
    { "MissingImageChannel", 112 },
    { "NegativeOrZeroImageSize", 113 },
    { "NonOS2HeaderSizeError", 114 },
    { "NotEnoughTiles", 115 },
    { "StaticPlanesValueNotEqualToOne", 116 },
    { "SubsamplingRequiresEvenWidth", 117 },
    { "TooMuchImageDataInFile", 118 },
    { "UnableToReadColormapFromDumpFile", 119 },
    { "UnableToReadColorProfile", 120 },
    { "UnableToReadExtensionBlock", 121 },
    { "UnableToReadGenericProfile", 122 },
    { "UnableToReadImageData", 123 },
    { "UnableToReadImageHeader", 124 },
    { "UnableToReadIPTCProfile", 125 },
    { "UnableToReadPixmapFromDumpFile", 126 },
    { "UnableToReadSubImageData", 127 },
    { "UnableToReadVIDImage", 128 },
    { "UnableToReadWindowNameFromDumpFile", 129 },
    { "UnableToRunlengthDecodeImage", 130 },
    { "UnableToUncompressImage", 131 },
    { "UnexpectedEndOfFile", 132 },
    { "UnexpectedSamplingFactor", 133 },
    { "UnknownPatternType", 134 },
    { "UnrecognizedBitsPerPixel", 135 },
    { "UnrecognizedImageCompression", 136 },
    { "UnrecognizedXWDHeader", 137 },
    { "UnableToPersistKey", 138 },
    { "CompressionNotValid", 139 },
    { "ImproperImageHeader", 140 },
    { "LengthAndFilesizeDoNotMatch", 141 },
    { "NegativeOrZeroImageSize", 142 },
    { "NonOS2HeaderSizeError", 143 },
    { "SkipToSyncByte", 144 },
    { "StaticPlanesValueNotEqualToOne", 145 },
    { "UnrecognizedBitsPerPixel", 146 },
    { "UnrecognizedImageCompression", 147 },
    { "UnrecognizedNumberOfColors", 148 },
    { "DelegateFailed", 149 },
    { "FailedToAllocateArgumentList", 150 },
    { "FailedToAllocateGhostscriptInterpreter", 151 },
    { "FailedToComputeOutputSize", 152 },
    { "FailedToRenderFile", 153 },
    { "FailedToScanFile", 154 },
    { "NoTagFound", 155 },
    { "PostscriptDelegateFailed", 156 },
    { "UnableToCreateImage", 157 },
    { "UnableToCreateImageComponent", 158 },
    { "UnableToDecodeImageFile", 159 },
    { "UnableToEncodeImageFile", 160 },
    { "UnableToInitializeFPXLibrary", 161 },
    { "UnableToInitializeWMFLibrary", 162 },
    { "UnableToManageJP2Stream", 163 },
    { "UnableToWriteSVGFormat", 164 },
    { "Default", 165 },
    { "Default", 166 },
    { "AlreadyPushingPatternDefinition", 167 },
    { "NonconformingDrawingPrimitiveDefinition", 168 },
    { "TooManyCoordinates", 169 },
    { "UnableToPrint", 170 },
    { "UnbalancedGraphicContextPushPop", 171 },
    { "Default", 172 },
    { "NotARelativeURL", 173 },
    { "NotCurrentlyPushingPatternDefinition", 174 },
    { "URLNotFound", 175 },
    { "UnableToCreateTemporaryFile", 176 },
    { "UnableToOpenFile", 177 },
    { "UnableToWriteFile", 178 },
    { "Default", 179 },
    { "Default", 180 },
    { "AngleIsDiscontinuous", 181 },
    { "ColorspaceColorProfileMismatch", 182 },
    { "ImageColorspaceDiffers", 183 },
    { "ImageColorspaceMismatch", 184 },
    { "ImageDifferenceExceedsLimit", 185 },
    { "ImageDoesNotContainResolution", 186 },
    { "ImageOpacityDiffers", 187 },
    { "ImageSequenceIsRequired", 188 },
    { "ImageSizeDiffers", 189 },
    { "InvalidColormapIndex", 190 },
    { "LeftAndRightImageSizesDiffer", 191 },
    { "NoImagesWereFound", 192 },
    { "NoImagesWereLoaded", 193 },
    { "NoLocaleImageAttribute", 194 },
    { "TooManyClusters", 195 },
    { "UnableToAppendImage", 196 },
    { "UnableToAssignProfile", 197 },
    { "UnableToAverageImage", 198 },
    { "UnableToCoalesceImage", 199 },
    { "UnableToCompareImages", 200 },
    { "UnableToCreateImageMosaic", 201 },
    { "UnableToCreateStereoImage", 202 },
    { "UnableToDeconstructImageSequence", 203 },
    { "UnableToFlattenImage", 204 },
    { "UnableToGetClipMask", 205 },
    { "UnableToHandleImageChannel", 206 },
    { "UnableToResizeImage", 207 },
    { "UnableToSegmentImage", 208 },
    { "UnableToSetClipMask", 209 },
    { "UnableToShearImage", 210 },
    { "WidthOrHeightExceedsLimit", 211 },
    { "UnableToPersistKey", 212 },
    { "Default", 213 },
    { "DPSLibraryIsNotAvailable", 214 },
    { "FPXLibraryIsNotAvailable", 215 },
    { "FreeTypeLibraryIsNotAvailable", 216 },
    { "JPEGLibraryIsNotAvailable", 217 },
    { "LCMSLibraryIsNotAvailable", 218 },
    { "LZWEncodingNotEnabled", 219 },
    { "NoDecodeDelegateForThisImageFormat", 220 },
    { "NoEncodeDelegateForThisImageFormat", 221 },
    { "TIFFLibraryIsNotAvailable", 222 },
    { "XMLLibraryIsNotAvailable", 223 },
    { "XWindowLibraryIsNotAvailable", 224 },
    { "ZipLibraryIsNotAvailable", 225 },
    { "Default", 226 },
    { "Default", 227 },
    { "FailedToCloseModule", 228 },
    { "FailedToFindSymbol", 229 },
    { "UnableToLoadModule", 230 },
    { "UnableToRegisterImageFormat", 231 },
    { "UnrecognizedModule", 232 },
    { "UnableToInitializeModuleLoader", 233 },
    { "Default", 234 },
    { "Default", 235 },
    { "Default", 236 },
    { "UserRequestedTerminationBySignal", 237 },
    { "Default", 238 },
    { "BevelWidthIsNegative", 239 },
    { "ColorSeparatedImageRequired", 240 },
    { "FrameIsLessThanImageSize", 241 },
    { "GeometryDimensionsAreZero", 242 },
    { "GeometryDoesNotContainImage", 243 },
    { "HaldClutImageDimensionsInvalid", 244 },
    { "ImagesAreNotTheSameSize", 245 },
    { "ImageSizeMustExceedBevelWidth", 246 },
    { "ImageSmallerThanKernelWidth", 247 },
    { "ImageSmallerThanRadius", 248 },
    { "ImageWidthsOrHeightsDiffer", 249 },
    { "InputImagesAlreadySpecified", 250 },
    { "InvalidSubimageSpecification", 251 },
    { "KernelRadiusIsTooSmall", 252 },
    { "KernelWidthMustBeAnOddNumber", 253 },
    { "MatrixIsNotSquare", 254 },
    { "MatrixOrderOutOfRange", 255 },
    { "MissingAnImageFilename", 256 },
    { "MissingArgument", 257 },
    { "MustSpecifyAnImageName", 258 },
    { "MustSpecifyImageSize", 259 },
    { "NoBlobDefined", 260 },
    { "NoImagesDefined", 261 },
    { "NonzeroWidthAndHeightRequired", 262 },
    { "NoProfileNameWasGiven", 263 },
    { "NullBlobArgument", 264 },
    { "ReferenceImageRequired", 265 },
    { "ReferenceIsNotMyType", 266 },
    { "RegionAreaExceedsLimit", 267 },
    { "RequestDidNotReturnAnImage", 268 },
    { "SteganoImageRequired", 269 },
    { "StereoImageRequired", 270 },
    { "SubimageSpecificationReturnsNoImages", 271 },
    { "UnableToAddOrRemoveProfile", 272 },
    { "UnableToAverageImageSequence", 273 },
    { "UnableToBlurImage", 274 },
    { "UnableToChopImage", 275 },
    { "UnableToColorMatrixImage", 276 },
    { "UnableToConstituteImage", 277 },
    { "UnableToConvolveImage", 278 },
    { "UnableToEdgeImage", 279 },
    { "UnableToEqualizeImage", 280 },
    { "UnableToFilterImage", 281 },
    { "UnableToFormatImageMetadata", 282 },
    { "UnableToFrameImage", 283 },
    { "UnableToOilPaintImage", 284 },
    { "UnableToPaintImage", 285 },
    { "UnableToRaiseImage", 286 },
    { "UnableToSharpenImage", 287 },
    { "UnableToThresholdImage", 288 },
    { "UnableToWaveImage", 289 },
    { "UnrecognizedAttribute", 290 },
    { "UnrecognizedChannelType", 291 },
    { "UnrecognizedColor", 292 },
    { "UnrecognizedColormapType", 293 },
    { "UnrecognizedColorspace", 294 },
    { "UnrecognizedCommand", 295 },
    { "UnrecognizedComposeOperator", 296 },
    { "UnrecognizedDisposeMethod", 297 },
    { "UnrecognizedElement", 298 },
    { "UnrecognizedEndianType", 299 },
    { "UnrecognizedGravityType", 300 },
    { "UnrecognizedHighlightStyle", 301 },
    { "UnrecognizedImageCompression", 302 },
    { "UnrecognizedImageFilter", 303 },
    { "UnrecognizedImageFormat", 304 },
    { "UnrecognizedImageMode", 305 },
    { "UnrecognizedImageType", 306 },
    { "UnrecognizedIntentType", 307 },
    { "UnrecognizedInterlaceType", 308 },
    { "UnrecognizedListType", 309 },
    { "UnrecognizedMetric", 310 },
    { "UnrecognizedModeType", 311 },
    { "UnrecognizedNoiseType", 312 },
    { "UnrecognizedOperator", 313 },
    { "UnrecognizedOption", 314 },
    { "UnrecognizedPerlMagickMethod", 315 },
    { "UnrecognizedPixelMap", 316 },
    { "UnrecognizedPreviewType", 317 },
    { "UnrecognizedResourceType", 318 },
    { "UnrecognizedType", 319 },
    { "UnrecognizedUnitsType", 320 },
    { "UnrecognizedVirtualPixelMethod", 321 },
    { "UnsupportedSamplingFactor", 322 },
    { "UsageError", 323 },
    { "InvalidColorspaceType", 324 },
    { "InvalidEndianType", 325 },
    { "InvalidImageType", 326 },
    { "InvalidInterlaceType", 327 },
    { "MissingAnImageFilename", 328 },
    { "MissingArgument", 329 },
    { "NoImagesWereLoaded", 330 },
    { "OptionLengthExceedsLimit", 331 },
    { "RequestDidNotReturnAnImage", 332 },
    { "UnableToOpenXServer", 333 },
    { "UnableToPersistKey", 334 },
    { "UnrecognizedColormapType", 335 },
    { "UnrecognizedColorspaceType", 336 },
    { "UnrecognizedDisposeMethod", 337 },
    { "UnrecognizedEndianType", 338 },
    { "UnrecognizedFilterType", 339 },
    { "UnrecognizedImageCompressionType", 340 },
    { "UnrecognizedImageType", 341 },
    { "UnrecognizedInterlaceType", 342 },
    { "UnrecognizedOption", 343 },
    { "UnrecognizedResourceType", 344 },
    { "UnrecognizedVirtualPixelMethod", 345 },
    { "UnrecognizedColor", 346 },
    { "ImageExpected", 347 },
    { "ImageInfoExpected", 348 },
    { "StructureSizeMismatch", 349 },
    { "UnableToGetRegistryID", 350 },
    { "UnableToLocateImage", 351 },
    { "UnableToSetRegistry", 352 },
    { "Default", 353 },
    { "Default", 354 },
    { "CacheResourcesExhausted", 355 },
    { "ImagePixelLimitExceeded", 356 },
    { "MemoryAllocationFailed", 357 },
    { "NoPixelsDefinedInCache", 358 },
    { "PixelCacheAllocationFailed", 359 },
    { "UnableToAddColorProfile", 360 },
    { "UnableToAddGenericProfile", 361 },
    { "UnableToAddIPTCProfile", 362 },
    { "UnableToAllocateCoefficients", 363 },
    { "UnableToAllocateColormap", 364 },
    { "UnableToAllocateICCProfile", 365 },
    { "UnableToAllocateString", 366 },
    { "UnableToAnnotateImage", 367 },
    { "UnableToAverageImageSequence", 368 },
    { "UnableToCloneDrawingWand", 369 },
    { "UnableToCloneImage", 370 },
    { "UnableToComputeImageSignature", 371 },
    { "UnableToConstituteImage", 372 },
    { "UnableToConvertFont", 373 },
    { "UnableToConvertStringToTokens", 374 },
    { "UnableToCreateColormap", 375 },
    { "UnableToCreateColorTransform", 376 },
    { "UnableToCreateCommandWidget", 377 },
    { "UnableToCreateImageGroup", 378 },
    { "UnableToCreateImageMontage", 379 },
    { "UnableToCreateXWindow", 380 },
    { "UnableToCropImage", 381 },
    { "UnableToDespeckleImage", 382 },
    { "UnableToDetermineImageClass", 383 },
    { "UnableToDetermineTheNumberOfImageColors", 384 },
    { "UnableToDitherImage", 385 },
    { "UnableToDrawOnImage", 386 },
    { "UnableToEdgeImage", 387 },
    { "UnableToEmbossImage", 388 },
    { "UnableToEnhanceImage", 389 },
    { "UnableToFloodfillImage", 390 },
    { "UnableToGammaCorrectImage", 391 },
    { "UnableToGetBestIconSize", 392 },
    { "UnableToGetFromRegistry", 393 },
    { "UnableToGetPackageInfo", 394 },
    { "UnableToLevelImage", 395 },
    { "UnableToMagnifyImage", 396 },
    { "UnableToManageColor", 397 },
    { "UnableToMapImage", 398 },
    { "UnableToMapImageSequence", 399 },
    { "UnableToMedianFilterImage", 400 },
    { "UnableToMotionBlurImage", 401 },
    { "UnableToNoiseFilterImage", 402 },
    { "UnableToNormalizeImage", 403 },
    { "UnableToOpenColorProfile", 404 },
    { "UnableToQuantizeImage", 405 },
    { "UnableToQuantizeImageSequence", 406 },
    { "UnableToReadTextChunk", 407 },
    { "UnableToReadXImage", 408 },
    { "UnableToReadXServerColormap", 409 },
    { "UnableToResizeImage", 410 },
    { "UnableToRotateImage", 411 },
    { "UnableToSampleImage", 412 },
    { "UnableToScaleImage", 413 },
    { "UnableToSelectImage", 414 },
    { "UnableToSharpenImage", 415 },
    { "UnableToShaveImage", 416 },
    { "UnableToShearImage", 417 },
    { "UnableToSortImageColormap", 418 },
    { "UnableToThresholdImage", 419 },
    { "UnableToTransformColorspace", 420 },
    { "MemoryAllocationFailed", 421 },
    { "SemaporeOperationFailed", 422 },
    { "UnableToAllocateAscii85Info", 423 },
    { "UnableToAllocateCacheInfo", 424 },
    { "UnableToAllocateCacheView", 425 },
    { "UnableToAllocateColorInfo", 426 },
    { "UnableToAllocateDashPattern", 427 },
    { "UnableToAllocateDelegateInfo", 428 },
    { "UnableToAllocateDerivatives", 429 },
    { "UnableToAllocateDrawContext", 430 },
    { "UnableToAllocateDrawInfo", 431 },
    { "UnableToAllocateDrawingWand", 432 },
    { "UnableToAllocateGammaMap", 433 },
    { "UnableToAllocateImage", 434 },
    { "UnableToAllocateImagePixels", 435 },
    { "UnableToAllocateLogInfo", 436 },
    { "UnableToAllocateMagicInfo", 437 },
    { "UnableToAllocateMagickInfo", 438 },
    { "UnableToAllocateModuleInfo", 439 },
    { "UnableToAllocateMontageInfo", 440 },
    { "UnableToAllocateQuantizeInfo", 441 },
    { "UnableToAllocateRandomKernel", 442 },
    { "UnableToAllocateRegistryInfo", 443 },
    { "UnableToAllocateSemaphoreInfo", 444 },
    { "UnableToAllocateString", 445 },
    { "UnableToAllocateTypeInfo", 446 },
    { "UnableToAllocateWand", 447 },
    { "UnableToAnimateImageSequence", 448 },
    { "UnableToCloneBlobInfo", 449 },
    { "UnableToCloneCacheInfo", 450 },
    { "UnableToCloneImage", 451 },
    { "UnableToCloneImageInfo", 452 },
    { "UnableToConcatenateString", 453 },
    { "UnableToConvertText", 454 },
    { "UnableToCreateColormap", 455 },
    { "UnableToDestroySemaphore", 456 },
    { "UnableToDisplayImage", 457 },
    { "UnableToEscapeString", 458 },
    { "UnableToInitializeSemaphore", 459 },
    { "UnableToInterpretMSLImage", 460 },
    { "UnableToLockSemaphore", 461 },
    { "UnableToUnlockSemaphore", 462 },
    { "MemoryAllocationFailed", 463 },
    { "ImageDoesNotContainTheStreamGeometry", 464 },
    { "NoStreamHandlerIsDefined", 465 },
    { "PixelCacheIsNotOpen", 466 },
    { "UnableToAcquirePixelStream", 467 },
    { "UnableToSetPixelStream", 468 },
    { "UnableToSyncPixelStream", 469 },
    { "Default", 470 },
    { "Default", 471 },
    { "FontSubstitutionRequired", 472 },
    { "UnableToGetTypeMetrics", 473 },
    { "UnableToInitializeFreetypeLibrary", 474 },
    { "UnableToReadFont", 475 },
    { "UnrecognizedFontEncoding", 476 },
    { "Default", 477 },
    { "Default", 478 },
    { "InvalidColormapIndex", 479 },
    { "WandAPINotImplemented", 480 },
    { "WandContainsNoImageIndexs", 481 },
    { "WandContainsNoImages", 482 },
    { "ColorIsNotKnownToServer", 483 },
    { "NoWindowWithSpecifiedIDExists", 484 },
    { "StandardColormapIsNotInitialized", 485 },
    { "UnableToConnectToRemoteDisplay", 486 },
    { "UnableToCreateBitmap", 487 },
    { "UnableToCreateColormap", 488 },
    { "UnableToCreatePixmap", 489 },
    { "UnableToCreateProperty", 490 },
    { "UnableToCreateStandardColormap", 491 },
    { "UnableToDisplayImageInfo", 492 },
    { "UnableToGetProperty", 493 },
    { "UnableToGetStandardColormap", 494 },
    { "UnableToGetVisual", 495 },
    { "UnableToGrabMouse", 496 },
    { "UnableToLoadFont", 497 },
    { "UnableToMatchVisualToStandardColormap", 498 },
    { "UnableToOpenXServer", 499 },
    { "UnableToReadXAttributes", 500 },
    { "UnableToReadXWindowImage", 501 },
    { "UnrecognizedColormapType", 502 },
    { "UnrecognizedGravityType", 503 },
    { "UnrecognizedVisualSpecifier", 504 },
    { "UnableToAllocateXHints", 505 },
    { "UnableToCreateCursor", 506 },
    { "UnableToCreateGraphicContext", 507 },
    { "UnableToCreateStandardColormap", 508 },
    { "UnableToCreateTextProperty", 509 },
    { "UnableToCreateXImage", 510 },
    { "UnableToCreateXPixmap", 511 },
    { "UnableToCreateXWindow", 512 },
    { "UnableToDisplayImage", 513 },
    { "UnableToDitherImage", 514 },
    { "UnableToGetPixelInfo", 515 },
    { "UnableToGetVisual", 516 },
    { "UnableToLoadFont", 517 },
    { "UnableToMakeXWindow", 518 },
    { "UnableToOpenXServer", 519 },
    { "UnableToViewFonts", 520 },
    { "UnableToGetVisual", 521 },
    { "UsingDefaultVisual", 522 },
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
    "Unable to change to working directory",
    "Unable to get current working directory",
    "Unable to restore current working directory",
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
    "Length and filesize do not match",
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
    "too many coordinates",
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
    "image colorspace mismatch",
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
    "unable to handle image channel",
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
    "default error",
    "default error",
    "User requested termination (via signal)",
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
    "Region area exceeds implementation limit",
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
    "Semaphore operation failed",
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
    "unable to destroy semaphore",
    "unable to display image",
    "unable to escape string",
    "unable to initialize semaphore",
    "unable to interpret MSL image",
    "unable to lock semaphore",
    "unable to unlock semaphore",
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
