(window.webpackJsonp=window.webpackJsonp||[]).push([["2d11"],{YD4I:function(t,e,a){(window.__NEXT_P=window.__NEXT_P||[]).push(["/download",function(){var t=a("bNlt");return{page:t.default||t}}])},bNlt:function(t,e,a){"use strict";a.r(e);var n=a("0iUn"),r=a("sLSF"),o=a("MI3g"),d=a("a7VT"),i=a("Tit0"),l=a("q1tI"),c=a.n(l),u=(a("nOHt"),a("b0oO")),s=a("iXVO"),w=a("MX0m"),m=a.n(w),p=a("obyI"),f=function(t,e){return t/e*100},v=function(t){var e=t.value,a=t.max;return c.a.createElement("div",{className:m.a.dynamic([["3777254888",[p.a.darkGray,f(e,a),p.a.generalRed]]])+" parent"},c.a.createElement("div",{className:m.a.dynamic([["3777254888",[p.a.darkGray,f(e,a),p.a.generalRed]]])+" value"}),c.a.createElement(m.a,{id:"3777254888",dynamic:[p.a.darkGray,f(e,a),p.a.generalRed]},[".parent.__jsx-style-dynamic-selector{margin-top:60px;width:100%;height:4px;background-color:".concat(p.a.darkGray,";border-radius:4px;}"),".value.__jsx-style-dynamic-selector{height:4px;border-radius:4px;width:".concat(f(e,a),"%;background-color:").concat(p.a.generalRed,";-webkit-transition:width 1s ease-out;transition:width 1s ease-out;}")]))};a.d(e,"default",function(){return b});var b=function(t){function e(t){var a;return Object(n.default)(this,e),(a=Object(o.default)(this,Object(d.default)(e).call(this,t))).state={current:0,total:0},a.stage2=!1,a}return Object(i.default)(e,t),Object(r.default)(e,[{key:"componentDidMount",value:function(){var t=this;this.interval=setInterval(function(){t.setState({current:window.oakwood.data.bytesRead,total:window.oakwood.data.bytesTotal})},100)}},{key:"componentWillUnmount",value:function(){clearInterval(this.interval)}},{key:"componentDidUpdate",value:function(){window.oakwood.data.updateDownloaded}},{key:"render",value:function(){return c.a.createElement(l.Fragment,null,c.a.createElement(u.a,null),c.a.createElement(s.b,null,"Downloading updates"),c.a.createElement(v,{value:this.state.current,max:this.state.total}))}}]),e}(l.Component)}},[["YD4I","5d41","9da1"]]]);