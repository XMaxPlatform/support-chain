


function init(code,type)
{
	StoreRecord(StrToName("xmax"),StrToName("testdata"),StrToName("testacc"),999);
}

function apply(code ,type)
{
	if(StrIsName("ercc",code))
	{
		if(StrIsName("transfer",type))
		{
			transfer(code,type);
		}
	}
}

function transfer(code,type)
{
	var amount = GetMsgData(code,type,"quantity","int")
	
	StoreRecord(StrToName("xmax"),StrToName("testdata"),StrToName("testdata"),amount);
}