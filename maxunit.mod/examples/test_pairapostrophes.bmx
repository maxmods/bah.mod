SuperStrict

Framework BaH.MaxUnit

Import "pairapostrophes.bmx"

New TTestSuite.run()

Type PairTest Extends TTest

	Method testNone() { test }
	
		Local text:String = "nothing to see here"
		
		assertEquals(text, PairApostrophes(text))
	
	End Method

	Method testPairedNoChange() { test }
	
		Local text:String = "What''s it all about?"
		
		assertEquals(text, PairApostrophes(text))
	
	End Method

	Method testSingleToPair() { test }
	
		Local text:String = "Don't do it"
		Local expectedText:String = "Don''t do it"
		
		assertEquals(expectedText, PairApostrophes(text))
	
	End Method

	Method testTrebleToQuad() { test }
	
		Local text:String = "abc'''def"
		Local expectedText:String = "abc''''def"
		
		assertEquals(expectedText, PairApostrophes(text))
	
	End Method

	Method testMixed() { test }
	
		Local text:String = "There''s nothin' doin''. abc'''e'."
		Local expectedText:String = "There''s nothin'' doin''. abc''''e''."
		
		assertEquals(expectedText, PairApostrophes(text))
	
	End Method

End Type
