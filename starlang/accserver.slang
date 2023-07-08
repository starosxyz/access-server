<?xml version="1.0" encoding="UTF-8"?>
<starlang xmlns="http://www.staros.xyz/starcore" scope="application">
	<logconfig dest="stdout"/>
        <pyvariable name="appname"/>
	<events>
		<onevent event="bundleapp.startup">
			<function>
					<log>vnet bundleapp.startup</log>
					<log>test js</log>
					<long name="i"/>
					<for var="i" from="1" to="20">
							<log><![CDATA[ test for i= <%=context.i.toString()%>]]></log>
					</for>
			</function>
		</onevent>
		<onevent event="~bundleapp.shutdown">
			<function>
					<log>bundleapp.SHUTDOWN</log>
			</function>
		</onevent>
		<onevent event="*">
			<function>
				<log level="1"><![CDATA[ invalid event <%=event.getName()%>]]> </log>
				<log level="1"><![CDATA[ invalid param <%=event.getParam("eventdata").toString()%>]]> </log>
			</function>
		</onevent>
	</events>
</starlang>