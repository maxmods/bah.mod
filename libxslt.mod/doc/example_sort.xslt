<?xml version="1.0" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="/">
     <html><head><title>Tutorial Example</title></head><body>

     <xsl:apply-templates select="famous-persons/persons">
       <xsl:sort select="@category" />
     </xsl:apply-templates>

     </body></html>
  </xsl:template>

  <xsl:template match="persons">

    <h2><xsl:value-of select="@category" /></h2>
    <ul>

       <xsl:apply-templates select="person">
         <xsl:sort select="name"      />
         <xsl:sort select="firstname" />
       </xsl:apply-templates>

    </ul>

  </xsl:template>

  <xsl:template match="person">

        <xsl:text disable-output-escaping="yes">
            &lt;li&gt;
        </xsl:text>

        <b><xsl:value-of select="name"      /></b>
           <xsl:value-of select="firstname" />

  </xsl:template>

</xsl:stylesheet>
