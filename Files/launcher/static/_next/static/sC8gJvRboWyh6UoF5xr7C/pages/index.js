(window.webpackJsonp=window.webpackJsonp||[]).push([["ce6e"],{Lev9:function(t,e,n){"use strict";var o=n("nOHt"),a=n.n(o);e.a=function(t,e){t&&t.res?(t.res.writeHead(303,{Location:e}),t.res.end()):a.a.replace(e)}},RNiq:function(t,e,n){"use strict";n.r(e),n.d(e,"default",function(){return p});var o=n("0iUn"),a=n("sLSF"),i=n("MI3g"),l=n("a7VT"),c=n("Tit0"),u=n("9Jkg"),d=n.n(u),r=n("q1tI"),s=n.n(r),w=(n("nOHt"),n("b0oO")),f=n("iXVO"),v=n("Lev9");console.dump=function(t){window.JSON&&window.JSON.stringify?console.log(d()(t)):console.log(t)};var p=function(t){function e(t){var n;return Object(o.default)(this,e),(n=Object(i.default)(this,Object(l.default)(e).call(this,t))).state={dots:"."},n}return Object(c.default)(e,t),Object(a.default)(e,[{key:"componentDidMount",value:function(){var t=this;try{window.external.invoke("init")}catch(e){}this.interval=setInterval(function(){var e="."==t.state.dots?"..":".."==t.state.dots?"...":(t.state.dots,".");t.setState({dots:e})},500),setTimeout(function(){try{window.oakwood.data.loaded?(console.dump(window.oakwood),window.oakwood.checkForUpdates(),window.oakwood.data.needsUpdate?Object(v.a)(null,"/update"):Object(v.a)(null,"/play")):Object(v.a)(null,"/setup")}catch(e){console.log("no context in browser mode"),setTimeout(function(){return Object(v.a)(null,"/news")},2e3)}},1e3)}},{key:"componentWillUnmount",value:function(){this.interval&&clearInterval(this.interval)}},{key:"render",value:function(){return s.a.createElement(r.Fragment,null,s.a.createElement(w.a,null),s.a.createElement(f.b,null,"Initializing ",this.state.dots))}}]),e}(r.Component)},vlRD:function(t,e,n){(window.__NEXT_P=window.__NEXT_P||[]).push(["/",function(){var t=n("RNiq");return{page:t.default||t}}])}},[["vlRD","5d41","9da1"]]]);